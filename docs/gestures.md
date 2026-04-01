# gesture reference

complete list of gestures the demuxer init supports and their default mappings.

---

## IMU gestures (wrist/arm)

| gesture | motion | default action |
|---------|--------|---------------|
| jerk forward | quick arm push forward | melee / dash / push |
| jerk backward | quick arm pull back | block / pull |
| twist left | wrist rotate counterclockwise | swap weapon |
| twist right | wrist rotate clockwise | reload |
| tilt left | arm tilt left | lean left |
| tilt right | arm tilt right | lean right |
| tilt up | pitch arm up | tilt up |
| tilt down | pitch arm down | tilt down |

all IMU gestures have a cooldown timer (default 400ms) to prevent accidental rapid re-trigger. adjustable per profile.

---

## flex gestures (fingers)

| gesture | motion | default action |
|---------|--------|---------------|
| grip | all fingers (index–pinky) curl ≥ threshold | grab / hold |
| pinch | thumb + index curl ≥ threshold | interact |
| individual curl | any single finger bend | profile-defined |

flex thresholds are normalized 0.0–1.0. default grip threshold = 0.70.

---

## foil contacts (palm pads)

| contact | default mapping |
|---------|----------------|
| A | confirm / menu select |
| B | back / cancel |
| C | push-to-talk |
| D | modifier (hold to change gesture meanings) |
| RECAL | hold 3 seconds → triggers full recalibration |

---

## special modes

### wheel mode
arm-based steering. grab any round object as a wheel.

- X axis (left/right tilt) = primary steering
- vertical arm movement blended in to improve freeform object tracking
- output range: -1.0 (full left) to 1.0 (full right)
- calibrate: set center → set left lock → set right lock
- adjustable deadzone per profile

reliability tiers:
- **fixed wheel** (sim rig, mounted) = most reliable
- **freeform object** (frisbee, plate, book) = good for casual
- **air steering** (no object) = drifty, just for fun

wheel mode is **exclusive** — other gestures disabled while active.

### finger-walk mode
use index + middle finger as character legs.

| input | action |
|-------|--------|
| alternating index/middle taps | move forward |
| tap speed < 1.5 taps/sec | walk |
| tap speed > 3.5 taps/sec | run |
| wrist twist left | turn left |
| wrist twist right | turn right |

finger-walk is **standalone** — cannot combine with other gesture modes.
debounce logic prevents accidental double-fire on fast taps.

---

## modifier behavior

holding **foil D** (modifier) while doing a gesture maps to a secondary action.
secondary mappings are profile-defined.

example (default profile with modifier held):
- jerk forward → ability 1 (instead of melee)
- grip → crouch (instead of grab)

---

## calibration flow

1. hand relaxed → baseline flex reading
2. full curl all fingers → max flex reading
3. wrist forward/back + twist test
4. wheel mode: set center → left lock → right lock
5. finger-walk tap test: slow tap → fast tap
6. save profile name

trigger recalibration anytime by holding **FOIL_RECAL** for 3 seconds.
