# Formula reference

Conventions (same as the simulator and ROS): x forward, y left, angles counter-clockwise positive, radians, yaw in −π…π.

## Basics

| What | Formula |
|---|---|
| Distance | `d = hypot(gx − x, gy − y)` |
| Bearing to goal | `β = atan2(gy − y, gx − x)` |
| Heading error | `α = wrap(β − yaw)` |
| Wrap angle | `while (a > π) a −= 2π; while (a < −π) a += 2π;` (or `atan2(sin a, cos a)`) |
| Yaw from quaternion | `yaw = atan2(2(w·z + x·y), 1 − 2(y² + z²))` (or `tf2::getYaw`) |

## Audi R8 constants

| Constant | Value |
|---|---|
| Steering ratio | 17.3 |
| Lock-to-lock revs | 3.2 |
| Max steer angle (wheel) | π × 3.2 / 17.3 ≈ **0.581 rad (33.3°)** |
| Wheelbase L | 2.65 m |
| Track width | 1.638 m |
| Wheel radius | 0.36 m |
| Max brake torque | 8000 Nm |
| Default throttle | 0.1 → top speed ≈ 2.91 m/s |

**Steering command = wheel angle δ × 17.3.** The simulator divides `steering_cmd` by the steering ratio, then clamps to the max wheel angle (`audibot_gazebo/src/AudibotInterfacePlugin.cpp` in pfms-support).

## Pure pursuit (A1 figure; Snider 2009)

With look-ahead/goal distance `ld` and heading error `α`:

```
δ        = atan( 2 L sin(α) / ld )        steering (wheel) angle
R_path   = ld / (2 sin α)                 radius of the circular arc the car follows
           (equivalently tan δ = L / R_path)
θ_arc    = 2α                             angle swept along the arc
s        = R_path · 2α = α · ld / sin α   arc length (α → 0 gives s = ld)
t        = s / 2.91                       time at default throttle
yaw_end  = wrap(yaw_start + 2α)           heading on arrival
reachable  ⇔  |δ| ≤ 0.581
```

Sanity checks: goal 10 m straight ahead → δ = 0, s = 10, t ≈ 3.44 s. A goal behind the car (|α| > π/2) is not reachable in one forward arc with this model.

## Bicycle model (for a mock simulator)

```
x   += v cos(yaw) dt
y   += v sin(yaw) dt
yaw += (v / L) tan(δ) dt
```

## Frames

**Body → world** (point `(bx, by)` in robot frame, robot at `(x, y, yaw)`):
```
wx = x + cos(yaw)·bx − sin(yaw)·by
wy = y + sin(yaw)·bx + cos(yaw)·by
```

**World → body** (used for quadcopter control: error in robot frame):
```
dx = gx − x,  dy = gy − y
forward =  cos(yaw)·dx + sin(yaw)·dy
left    = −sin(yaw)·dx + cos(yaw)·dy
```
Quadcopter P-control: scale `(forward, left)` to the desired speed, e.g. `v · (forward, left) / hypot(forward, left)`, and slow down near the goal.

## Laser scan → points

```
θ_i = angle_min + i · angle_increment
skip if !isfinite(r) or r < range_min or r > range_max
laser frame:  lx = r cos θ_i,  ly = r sin θ_i
car frame:    bx = lx + laser_offset_x,  by = ly      (Audi laser ≈ 1.5 m along x; confirm sign with a bag)
world frame:  body → world above
```

**Clustering (cones):** walk the points in scan order; start a new cluster when the gap to the previous point > ~0.3 m. Centre = mean of the cluster. Reject clusters that are too wide to be a cone.

## Geometry tests

**Which side of line a→b is p?** `cross = (bx−ax)(py−ay) − (by−ay)(px−ax)`; > 0 left, < 0 right.

**Point in convex polygon** (vertices in order): p is inside if `cross` has the same sign for every edge.

**Point near segment a→b** (obstruction checks): project `t = clamp(((p−a)·(b−a)) / |b−a|², 0, 1)`, closest point `a + t(b−a)`, compare distance to a clearance radius.

## Grand Tour (Project 3)

```
ground_height = drone_z − sonar_range
gradient      = |Δheight| / horizontal_distance      (traversable if < 0.03)
```

## TSP

- Brute force: fix the start, permute goal indices with `std::next_permutation` (n! orders: 5 → 120, 8 → 40,320, 10 → 3.6 M).
- Ackerman cost is **path dependent** (arrival heading changes the next arc), so evaluate each permutation by chaining legs.
- Larger n: nearest neighbour + 2-opt, or Held–Karp DP (O(n² 2ⁿ)).
