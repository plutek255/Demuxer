#include "demuxer.h"

// ── touchscreen.c ─────────────────────────────────────────────────────────────
// stub driver for the forearm dorsal touchscreen
// exact screen size/resolution TBD (see open questions in README)
// this module handles lock state, status rendering, and basic touch input routing

// ── config ────────────────────────────────────────────────────────────────────
// TODO: fill in once screen hardware is finalized
#define SCREEN_WIDTH    240     // placeholder
#define SCREEN_HEIGHT   135     // placeholder

// ── internal state ────────────────────────────────────────────────────────────
static bool _locked = false;

// stubs — replace with actual display driver (SPI/I2C LCD, e-ink, etc)
static void _display_clear(void) {}
static void _display_text(int x, int y, const char *str) { (void)x; (void)y; (void)str; }
static void _display_commit(void) {}

// ── init ──────────────────────────────────────────────────────────────────────
void touchscreen_init(void) {
    // init SPI/I2C display, set rotation, clear screen
    _display_clear();
    _display_text(10, 10, "demuxer init");
    _display_text(10, 30, "booting...");
    _display_commit();
    _locked = false;
}

// ── lock / unlock ─────────────────────────────────────────────────────────────
// lock prevents accidental touches during gameplay
void touchscreen_set_lock(bool locked) {
    _locked = locked;
    // optionally show a lock icon on screen
}

bool touchscreen_is_locked(void) {
    return _locked;
}

// ── status render ─────────────────────────────────────────────────────────────
// renders basic device status on the touchscreen
// call this periodically (e.g. every 500ms) not every frame
void touchscreen_render_status(const demuxer_state_t *state) {
    if (_locked) return;

    _display_clear();

    // battery %
    char bat_buf[16];
    // simple int-to-string since we may not have printf
    bat_buf[0] = 'B';
    bat_buf[1] = 'A';
    bat_buf[2] = 'T';
    bat_buf[3] = ':';
    bat_buf[4] = '0' + (state->battery_pct / 100) % 10;
    bat_buf[5] = '0' + (state->battery_pct / 10) % 10;
    bat_buf[6] = '0' + (state->battery_pct) % 10;
    bat_buf[7] = '%';
    bat_buf[8] = '\0';
    _display_text(10, 10, bat_buf);

    // calibration status
    _display_text(10, 30, state->calibrated ? "CAL: OK" : "CAL: NEEDED");

    // active mode
    const char *mode_str = "MODE: ?";
    switch (state->active_profile.mode) {
        case MODE_GAMEPAD:        mode_str = "MODE: GAMEPAD"; break;
        case MODE_KEYBOARD_MOUSE: mode_str = "MODE: KB/MS";   break;
        case MODE_CUSTOM:         mode_str = "MODE: CUSTOM";  break;
    }
    _display_text(10, 50, mode_str);

    // wheel / fingerwalk indicators
    if (state->wheel.active)      _display_text(10, 70, "WHEEL MODE");
    if (state->fingerwalk.active) _display_text(10, 70, "FINGERWALK");

    // profile name
    _display_text(10, 90, state->active_profile.name);

    _display_commit();
}

// ── touch input (stub) ────────────────────────────────────────────────────────
// returns a touched button index, or -1 if nothing / locked
// replace with actual touch controller read (e.g. FT6206, XPT2046)
int touchscreen_read_touch(void) {
    if (_locked) return -1;
    return -1; // stub
}
