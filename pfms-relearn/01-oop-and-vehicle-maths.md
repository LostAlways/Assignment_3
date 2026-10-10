# Week 2: Object-oriented design + vehicle maths

**Goal for the week:** the two foundations of A1: (1) abstract interfaces with derived classes, and (2) the geometry that says how an Ackerman car reaches a goal. You'll build both **without** the simulator, so they can be unit-tested.

Keep [formulas.md](formulas.md) open all week.

---

## Day 8: Classes, constructors, access specifiers

**Learn**
- `public` / `protected` / `private`, and *why*: protected is for things derived classes need.
- Default constructor, member initialiser lists, `= default`.
- `const` member functions.

**Do:** write a `Vehicle` class with private `x_, y_, yaw_`, a default constructor that zeroes them, getters marked `const`, and a `setPose(x,y,yaw)` that normalises yaw.

**Done when:** a test shows `Vehicle v;` starts at (0,0,0) and `setPose(0,0,4.0)` stores a yaw in −π…π.

---

## Day 9: Inheritance and polymorphism

**Learn**
- `virtual`, `override`, pure virtual (`= 0`), abstract classes.
- `virtual ~Base() = default;` and why it matters when deleting through a base pointer.
- Calling through `Base*` / `std::vector<Base*>` (this is exactly how A1's `Mission` holds platforms).

**Do:** abstract `Shape` with `area()` and `name()`. Derive `Circle`, `Rectangle`, `Triangle`. Store them in `std::vector<std::shared_ptr<Shape>>`, loop and print.

**Done when:** you can explain out loud why `Shape s;` doesn't compile and why that's the point.

---

## Day 10: Interface → base → derived (the A1 pattern)

This is the exact layering the assignments use:

```
ControllerInterface   (pure virtual, given, never modified)
        ▲
    Controller        (abstract base: shared data + shared logic)
      ▲      ▲
 Ackerman  Quadcopter (platform-specific maths + commands)
```

**Learn**
- Where to put a function: pure virtual in the interface, implemented once in `Controller` if *all* platforms do it the same way, or implemented in each derived class if it depends on the platform.
- The rule from the A1 FAQ: **the base class must not know about the derived classes.**

**Do:** on paper (or in `LOG.md`), sort these into "Controller" or "Ackerman/Quadcopter":
`getOdometry`, `distanceTravelled`, `timeInMotion`, `checkOriginToDestination` (can it reach, how far, how long), `sendCommand`, `getPlatformType`, `setTolerance`, `reachGoal`.

Then write a toy version: `PlatformInterface` → `Platform` → `Car` / `Drone`, where `Platform` stores the odometry and tolerance and each derived class implements `timeToGoal()` differently (car: arc length / 2.91 m/s; drone: straight line / 0.4 m/s).

**Done when:** a `std::vector<PlatformInterface*>` with one car and one drone prints different times for the same goal.

---

## Day 11: 2D geometry for robots

**Learn** (all in [formulas.md](formulas.md))
- `atan2(dy, dx)` and why not `atan`.
- Angle wrapping.
- Heading error α = bearing to goal − vehicle yaw.
- Quaternion → yaw (odometry gives orientation as a quaternion).

**Do:** functions `bearingTo(pose, goal)`, `headingError(pose, goal)`, `yawFromQuaternion(x,y,z,w)`. Unit-test with goals in all four quadrants, and with the vehicle facing backwards.

**Done when:** a goal directly behind the car gives |α| ≈ π, and a goal to the left gives positive α.

---

## Day 12: Pure pursuit and the bicycle model

**Learn**
- Watch: Jonathan Sprinkle's Ackermann steering video (linked in the A1 PDF).
- Read the "Platform: Audi R8" page of A1 and the pure-pursuit section of [formulas.md](formulas.md).

**Do:** implement
```cpp
double steeringAngle(double alpha, double ld, double wheelbase);   // δ
double turnRadius(double alpha, double ld);                        // R of the path
double arcLength(double alpha, double ld);                         // distance travelled
bool   reachable(double delta, double maxSteer);
double endYaw(double startYaw, double alpha);                      // start + 2α
```
using the Audi constants (wheelbase 2.65 m, steering ratio 17.3, lock-to-lock 3.2 revs).

**Hand-check one case first:** goal 10 m straight ahead → α=0 → δ=0, arc length = 10 m, time = 10/2.91 ≈ 3.44 s.

**Done when:** tests for straight-ahead, 30° left, 30° right, and an unreachable goal (α near 90° close to the car) all pass.

---

## Day 13: A mock platform (simulate without Gazebo)

The A1 rationale says engineers build a *mock* platform for testing without hardware. Do that now; it will save you hours.

**Do:** a `MockAckerman` that integrates the bicycle model every `dt`:
```
x   += v * cos(yaw) * dt
y   += v * sin(yaw) * dt
yaw += v / L * tan(δ) * dt
```
Feed it the δ from Day 12 for a goal and step until within 0.5 m (or 60 s).

**Done when:** the mock reaches a goal 10 m ahead and 5 m left, and the time it took is within ~1 s of your `arcLength / 2.91` prediction.

---

## Day 14: Buffer / review

- Self-check:
  1. Why must `ControllerInterface` have a virtual destructor?
  2. A goal needs δ = 0.7 rad. Max steer is ≈ 0.581 rad. What should `checkOriginToDestination` return, and why?
  3. Why does the car end up facing `start + 2α` after the arc?
- ⭐ Stretch: plot the mock's path (print CSV, open it in any spreadsheet or matplotlib) and check it's a circular arc.
