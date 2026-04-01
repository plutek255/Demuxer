/**
 * GitLab MCP Server — Cloudflare Worker
 * Implements MCP over Streamable HTTP (stateless JSON mode)
 * Compatible with claude.ai remote MCP connections
 */

const GITLAB_URL = "https://gitlab.com";

// ─── GitLab API helper ───────────────────────────────────────────────────────

async function glFetch(env, path, { method = "GET", body, params } = {}) {
  let url = `${GITLAB_URL}/api/v4${path}`;
  if (params) {
    const q = new URLSearchParams();
    for (const [k, v] of Object.entries(params)) {
      if (v !== undefined && v !== null) q.set(k, String(v));
    }
    const qs = q.toString();
    if (qs) url += "?" + qs;
  }
  const res = await fetch(url, {
    method,
    headers: {
      "PRIVATE-TOKEN": env.GITLAB_TOKEN,
      "Content-Type": "application/json",
    },
    ...(body ? { body: JSON.stringify(body) } : {}),
  });
  if (!res.ok) {
    const err = await res.text();
    let msg = err;
    try { msg = JSON.parse(err).message || err; } catch {}
    throw new Error(`GitLab API error (${res.status}): ${msg}`);
  }
  const headers = res.headers;
  const data = await res.json();
  return { data, headers };
}

async function glGet(env, path, params) {
  const { data } = await glFetch(env, path, { params });
  return data;
}

async function glGetPaged(env, path, params) {
  const { data, headers } = await glFetch(env, path, { params });
  return {
    data,
    page: parseInt(headers.get("x-page") ?? "1"),
    totalPages: parseInt(headers.get("x-total-pages") ?? "1"),
    total: parseInt(headers.get("x-total") ?? String(data.length)),
    nextPage: headers.get("x-next-page") ? parseInt(headers.get("x-next-page")) : null,
  };
}

async function glPost(env, path, body) {
  const { data } = await glFetch(env, path, { method: "POST", body });
  return data;
}

async function glPut(env, path, body) {
  const { data } = await glFetch(env, path, { method: "PUT", body });
  return data;
}

async function glDelete(env, path) {
  await glFetch(env, path, { method: "DELETE" });
}

const enc = (id) => encodeURIComponent(String(id));

// ─── Tool definitions ────────────────────────────────────────────────────────

const TOOLS = [
  // Projects
  { name: "gitlab_list_projects", description: "List GitLab projects accessible to the authenticated user. Filter by search term, ownership, membership, or visibility.", inputSchema: { type: "object", properties: { search: { type: "string" }, owned: { type: "boolean" }, membership: { type: "boolean" }, visibility: { type: "string", enum: ["public","internal","private"] }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_get_project", description: "Get full details of a specific project by ID or path (e.g. 'namespace/repo').", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] } } } },
  { name: "gitlab_create_project", description: "Create a new GitLab project.", inputSchema: { type: "object", required: ["name"], properties: { name: { type: "string" }, description: { type: "string" }, visibility: { type: "string", enum: ["public","internal","private"], default: "private" }, initialize_with_readme: { type: "boolean", default: false }, namespace_id: { type: "integer" } } } },
  // Issues
  { name: "gitlab_list_issues", description: "List issues for a project. Filter by state, labels, assignee, or search term.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, state: { type: "string", enum: ["opened","closed","all"], default: "opened" }, labels: { type: "string" }, assignee_username: { type: "string" }, search: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_get_issue", description: "Get full details of a specific issue by its IID (the number shown in the GitLab UI).", inputSchema: { type: "object", required: ["project_id","issue_iid"], properties: { project_id: { type: ["string","integer"] }, issue_iid: { type: "integer" } } } },
  { name: "gitlab_create_issue", description: "Create a new issue in a project.", inputSchema: { type: "object", required: ["project_id","title"], properties: { project_id: { type: ["string","integer"] }, title: { type: "string" }, description: { type: "string" }, labels: { type: "string" }, assignee_ids: { type: "array", items: { type: "integer" } }, milestone_id: { type: "integer" } } } },
  { name: "gitlab_update_issue", description: "Update an issue: change title, description, state (close/reopen), labels, or assignees.", inputSchema: { type: "object", required: ["project_id","issue_iid"], properties: { project_id: { type: ["string","integer"] }, issue_iid: { type: "integer" }, title: { type: "string" }, description: { type: "string" }, state_event: { type: "string", enum: ["close","reopen"] }, labels: { type: "string" }, assignee_ids: { type: "array", items: { type: "integer" } } } } },
  { name: "gitlab_create_issue_note", description: "Post a comment on an issue.", inputSchema: { type: "object", required: ["project_id","issue_iid","body"], properties: { project_id: { type: ["string","integer"] }, issue_iid: { type: "integer" }, body: { type: "string" } } } },
  // Merge Requests
  { name: "gitlab_list_merge_requests", description: "List merge requests for a project. Filter by state, branch, or search term.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, state: { type: "string", enum: ["opened","closed","locked","merged","all"], default: "opened" }, source_branch: { type: "string" }, target_branch: { type: "string" }, search: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_get_merge_request", description: "Get full details of a merge request by its IID.", inputSchema: { type: "object", required: ["project_id","mr_iid"], properties: { project_id: { type: ["string","integer"] }, mr_iid: { type: "integer" } } } },
  { name: "gitlab_create_merge_request", description: "Open a new merge request.", inputSchema: { type: "object", required: ["project_id","title","source_branch","target_branch"], properties: { project_id: { type: ["string","integer"] }, title: { type: "string" }, source_branch: { type: "string" }, target_branch: { type: "string" }, description: { type: "string" }, labels: { type: "string" }, assignee_ids: { type: "array", items: { type: "integer" } }, draft: { type: "boolean", default: false }, remove_source_branch: { type: "boolean", default: false } } } },
  { name: "gitlab_merge_merge_request", description: "Accept and merge an open merge request.", inputSchema: { type: "object", required: ["project_id","mr_iid"], properties: { project_id: { type: ["string","integer"] }, mr_iid: { type: "integer" }, merge_commit_message: { type: "string" }, squash: { type: "boolean", default: false }, should_remove_source_branch: { type: "boolean", default: false } } } },
  { name: "gitlab_create_mr_note", description: "Post a comment on a merge request.", inputSchema: { type: "object", required: ["project_id","mr_iid","body"], properties: { project_id: { type: ["string","integer"] }, mr_iid: { type: "integer" }, body: { type: "string" } } } },
  // Repository
  { name: "gitlab_list_branches", description: "List branches of a project.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, search: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_create_branch", description: "Create a new branch from a ref (branch name, tag, or commit SHA).", inputSchema: { type: "object", required: ["project_id","branch","ref"], properties: { project_id: { type: ["string","integer"] }, branch: { type: "string" }, ref: { type: "string" } } } },
  { name: "gitlab_delete_branch", description: "Delete a branch. Cannot delete protected or default branches.", inputSchema: { type: "object", required: ["project_id","branch"], properties: { project_id: { type: ["string","integer"] }, branch: { type: "string" } } } },
  { name: "gitlab_list_commits", description: "List commits in a project. Optionally filter by ref, date range, or file path.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, ref_name: { type: "string" }, since: { type: "string" }, until: { type: "string" }, path: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_get_file", description: "Get the decoded contents of a file from a repository.", inputSchema: { type: "object", required: ["project_id","file_path"], properties: { project_id: { type: ["string","integer"] }, file_path: { type: "string" }, ref: { type: "string" } } } },
  { name: "gitlab_list_pipelines", description: "List CI/CD pipelines for a project. Filter by status or ref.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, status: { type: "string", enum: ["created","waiting_for_resource","preparing","pending","running","success","failed","canceled","skipped","manual","scheduled"] }, ref: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_trigger_pipeline", description: "Trigger a new CI/CD pipeline on a branch or tag.", inputSchema: { type: "object", required: ["project_id","ref"], properties: { project_id: { type: ["string","integer"] }, ref: { type: "string" }, variables: { type: "object", additionalProperties: { type: "string" } } } } },
  // Users
  { name: "gitlab_get_current_user", description: "Get the profile of the currently authenticated user.", inputSchema: { type: "object", properties: {} } },
  { name: "gitlab_list_users", description: "Search for GitLab users by username, name, or email.", inputSchema: { type: "object", properties: { search: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
  { name: "gitlab_list_project_members", description: "List members of a project.", inputSchema: { type: "object", required: ["project_id"], properties: { project_id: { type: ["string","integer"] }, search: { type: "string" }, page: { type: "integer", default: 1 }, per_page: { type: "integer", default: 20 } } } },
];

// ─── Tool handlers ───────────────────────────────────────────────────────────

async function callTool(env, name, args) {
  switch (name) {
    // Projects
    case "gitlab_list_projects": {
      const r = await glGetPaged(env, "/projects", { ...args, order_by: "last_activity_at", sort: "desc" });
      return { projects: r.data.map(p => ({ id: p.id, name: p.name, path_with_namespace: p.path_with_namespace, visibility: p.visibility, default_branch: p.default_branch, description: p.description, star_count: p.star_count, open_issues_count: p.open_issues_count, web_url: p.web_url, last_activity_at: p.last_activity_at })), page: r.page, total_pages: r.totalPages, total: r.total };
    }
    case "gitlab_get_project":
      return glGet(env, `/projects/${enc(args.project_id)}`);
    case "gitlab_create_project": {
      const { project_id: _, ...body } = args;
      return glPost(env, "/projects", body);
    }
    // Issues
    case "gitlab_list_issues": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/issues`, params);
      return { issues: r.data.map(i => ({ id: i.id, iid: i.iid, title: i.title, state: i.state, labels: i.labels, author: i.author?.username, assignees: i.assignees?.map(a => a.username), created_at: i.created_at, updated_at: i.updated_at, web_url: i.web_url })), page: r.page, total_pages: r.totalPages, total: r.total };
    }
    case "gitlab_get_issue": {
      const { project_id, issue_iid } = args;
      return glGet(env, `/projects/${enc(project_id)}/issues/${issue_iid}`);
    }
    case "gitlab_create_issue": {
      const { project_id, ...body } = args;
      return glPost(env, `/projects/${enc(project_id)}/issues`, body);
    }
    case "gitlab_update_issue": {
      const { project_id, issue_iid, ...body } = args;
      return glPut(env, `/projects/${enc(project_id)}/issues/${issue_iid}`, body);
    }
    case "gitlab_create_issue_note": {
      const { project_id, issue_iid, body } = args;
      return glPost(env, `/projects/${enc(project_id)}/issues/${issue_iid}/notes`, { body });
    }
    // Merge Requests
    case "gitlab_list_merge_requests": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/merge_requests`, params);
      return { merge_requests: r.data.map(mr => ({ id: mr.id, iid: mr.iid, title: mr.title, state: mr.state, draft: mr.draft, source_branch: mr.source_branch, target_branch: mr.target_branch, author: mr.author?.username, assignees: mr.assignees?.map(a => a.username), labels: mr.labels, has_conflicts: mr.has_conflicts, created_at: mr.created_at, web_url: mr.web_url })), page: r.page, total_pages: r.totalPages, total: r.total };
    }
    case "gitlab_get_merge_request": {
      const { project_id, mr_iid } = args;
      return glGet(env, `/projects/${enc(project_id)}/merge_requests/${mr_iid}`);
    }
    case "gitlab_create_merge_request": {
      const { project_id, ...body } = args;
      return glPost(env, `/projects/${enc(project_id)}/merge_requests`, body);
    }
    case "gitlab_merge_merge_request": {
      const { project_id, mr_iid, ...body } = args;
      return glPut(env, `/projects/${enc(project_id)}/merge_requests/${mr_iid}/merge`, body);
    }
    case "gitlab_create_mr_note": {
      const { project_id, mr_iid, body } = args;
      return glPost(env, `/projects/${enc(project_id)}/merge_requests/${mr_iid}/notes`, { body });
    }
    // Repository
    case "gitlab_list_branches": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/repository/branches`, params);
      return { branches: r.data.map(b => ({ name: b.name, protected: b.protected, default: b.default, web_url: b.web_url, last_commit: { id: b.commit?.short_id, title: b.commit?.title, author: b.commit?.author_name, date: b.commit?.committed_date } })), page: r.page, total_pages: r.totalPages };
    }
    case "gitlab_create_branch": {
      const { project_id, ...body } = args;
      return glPost(env, `/projects/${enc(project_id)}/repository/branches`, body);
    }
    case "gitlab_delete_branch": {
      const { project_id, branch } = args;
      await glDelete(env, `/projects/${enc(project_id)}/repository/branches/${enc(branch)}`);
      return { message: `Branch "${branch}" deleted successfully` };
    }
    case "gitlab_list_commits": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/repository/commits`, params);
      return { commits: r.data.map(c => ({ id: c.short_id, full_id: c.id, title: c.title, author: c.author_name, date: c.committed_date, web_url: c.web_url })), page: r.page, total_pages: r.totalPages };
    }
    case "gitlab_get_file": {
      const { project_id, file_path, ref } = args;
      const d = await glGet(env, `/projects/${enc(project_id)}/repository/files/${enc(file_path)}`, ref ? { ref } : {});
      const decoded = atob(d.content);
      return { file_path: d.file_path, size: d.size, ref: d.ref, last_commit_id: d.last_commit_id, content: decoded };
    }
    case "gitlab_list_pipelines": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/pipelines`, params);
      return { pipelines: r.data.map(p => ({ id: p.id, status: p.status, ref: p.ref, sha: p.sha?.slice(0,8), source: p.source, duration: p.duration, created_at: p.created_at, web_url: p.web_url })), page: r.page, total_pages: r.totalPages };
    }
    case "gitlab_trigger_pipeline": {
      const { project_id, ref, variables } = args;
      const body = { ref };
      if (variables) body.variables = Object.entries(variables).map(([key, value]) => ({ key, value }));
      return glPost(env, `/projects/${enc(project_id)}/pipeline`, body);
    }
    // Users
    case "gitlab_get_current_user":
      return glGet(env, "/user");
    case "gitlab_list_users": {
      const r = await glGetPaged(env, "/users", args);
      return { users: r.data, page: r.page, total_pages: r.totalPages };
    }
    case "gitlab_list_project_members": {
      const { project_id, ...params } = args;
      const r = await glGetPaged(env, `/projects/${enc(project_id)}/members`, params);
      return { members: r.data, page: r.page, total: r.total };
    }
    default:
      throw new Error(`Unknown tool: ${name}`);
  }
}

// ─── MCP JSON-RPC handler ────────────────────────────────────────────────────

function mcpError(id, code, message) {
  return { jsonrpc: "2.0", id, error: { code, message } };
}

function mcpResult(id, result) {
  return { jsonrpc: "2.0", id, result };
}

async function handleMCP(request, env) {
  let body;
  try {
    body = await request.json();
  } catch {
    return new Response(JSON.stringify(mcpError(null, -32700, "Parse error")), { status: 400, headers: { "Content-Type": "application/json" } });
  }

  const { method, params, id } = body;

  try {
    let result;

    if (method === "initialize") {
      result = {
        protocolVersion: "2024-11-05",
        capabilities: { tools: {} },
        serverInfo: { name: "gitlab-mcp-server", version: "1.0.0" },
      };
    } else if (method === "tools/list") {
      result = { tools: TOOLS };
    } else if (method === "tools/call") {
      const { name, arguments: args = {} } = params;
      if (!env.GITLAB_TOKEN) throw new Error("GITLAB_TOKEN not configured");
      const data = await callTool(env, name, args);
      result = {
        content: [{ type: "text", text: JSON.stringify(data, null, 2) }],
      };
    } else if (method === "notifications/initialized") {
      // no-op notification
      return new Response(null, { status: 204 });
    } else {
      return new Response(JSON.stringify(mcpError(id, -32601, `Method not found: ${method}`)), { status: 200, headers: { "Content-Type": "application/json" } });
    }

    return new Response(JSON.stringify(mcpResult(id, result)), {
      status: 200,
      headers: {
        "Content-Type": "application/json",
        "Access-Control-Allow-Origin": "*",
      },
    });
  } catch (err) {
    const msg = err instanceof Error ? err.message : String(err);
    const isToolCall = method === "tools/call";
    if (isToolCall) {
      return new Response(JSON.stringify(mcpResult(id, { content: [{ type: "text", text: `Error: ${msg}` }], isError: true })), {
        status: 200,
        headers: { "Content-Type": "application/json", "Access-Control-Allow-Origin": "*" },
      });
    }
    return new Response(JSON.stringify(mcpError(id, -32603, msg)), {
      status: 200,
      headers: { "Content-Type": "application/json", "Access-Control-Allow-Origin": "*" },
    });
  }
}

// ─── Worker entry point ──────────────────────────────────────────────────────

export default {
  async fetch(request, env) {
    const url = new URL(request.url);

    // CORS preflight
    if (request.method === "OPTIONS") {
      return new Response(null, {
        headers: {
          "Access-Control-Allow-Origin": "*",
          "Access-Control-Allow-Methods": "POST, GET, OPTIONS",
          "Access-Control-Allow-Headers": "Content-Type, Authorization",
        },
      });
    }

    if (url.pathname === "/mcp" && request.method === "POST") {
      return handleMCP(request, env);
    }

    if (url.pathname === "/health") {
      return new Response(JSON.stringify({ status: "ok", server: "gitlab-mcp-server", tools: TOOLS.length }), {
        headers: { "Content-Type": "application/json" },
      });
    }

    return new Response("gitlab mcp server — POST to /mcp", { status: 200 });
  },
};
