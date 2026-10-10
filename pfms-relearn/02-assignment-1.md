# Week 3: Assignment 1 rebuild: Abstraction of Platform Classes

**Goal for the week:** re-implement A1 from the spec: `Controller` (base), `Ackerman` (derived), `Mission`. Drive the Audi to goals in the simulator, with unit tests and Doxygen.

Re-read the A1 PDF: *Code development*, *Assessment Criteria* and *FAQ*. The marking scheme is a great checklist for good design.

---

## Day 15: Set up the A1 project + talk to the simulator

**If you still have `a1_skeleton`:** copy it into `pfms-practice/a1/`, build it, and run the `a1_snippets` (e.g. `command_ugv`) with the A1 world launched.

**If you don't:** recreate the shape yourself. Write these as **pure virtual interfaces you promise not to change** (that's the discipline A1 tests):

```cpp
// controllerinterface.h  (your minimal version; the original had more)
class ControllerInterface {
public:
  virtual ~ControllerInterface() = default;
  virtual bool reachGoal() = 0;                       // drive to current goal, blocking (A1)
  virtual bool setGoal(Point goal) = 0;               // returns false if unreachable
  virtual bool checkOriginToDestination(Pose origin, Point goal,
                                        double& distance, double& time,
                                        Pose& estimatedGoalPose) = 0;
  virtual PlatformType getPlatformType() = 0;
  virtual double distanceToGoal() = 0;
  virtual double timeToGoal() = 0;
  virtual double distanceTravelled() = 0;
  virtual double timeInMotion() = 0;
  virtual bool setTolerance(double tolerance) = 0;
  virtual Odometry getOdometry() = 0;
};
```
and `MissionInterface` with `setGoals(...)`, `runMission()`, `setMissionObjective(...)`, `getDistanceTravelled()`, `getTimeMoving()`, `getPlatformGoalAssociation()`.

To talk to the simulator, use the **pipes / `PfmsConnector`** library from pfms-support (the `.deb` in the `2024` branch `packages/` folder), or skip ahead to week 6 and use plain ROS 2 topics. **Recommended:** for this week, develop against your `MockAckerman` from Day 13 and only connect to Gazebo on Day 19.

**Done when:** the project builds with an empty `Controller`, `Ackerman`, `Mission`, and a test that just constructs `Ackerman a;` passes.

---

## Day 16: `Controller` base class

**Do**
- `Controller : public ControllerInterface`, abstract (it can't be instantiated: give it a protected constructor or leave a pure virtual unimplemented).
- Put **shared** state here: current odometry, goal, tolerance, distance travelled, time in motion, platform type.
- Implement the **shared** functions here: `setTolerance`, `distanceTravelled`, `timeInMotion`, `getOdometry`, `getPlatformType`, `distanceToGoal`.
- **No `std::cout` in classes.** I/O only in `main` (a marked criterion).

**Done when:** a test using a tiny test-only `class FakePlatform : public Controller` can set tolerance and read odometry.

---

## Day 17: `Ackerman`: maths and `checkOriginToDestination`

**Do**
- Default constructor sets every Audi constant (wheelbase, steering ratio, lock-to-lock revs, max steer, throttle 0.1, top speed 2.91 m/s, max brake 8000 Nm).
- Move your Day 12 functions in (as private helpers or a small `ackerman_maths` library).
- `checkOriginToDestination`: compute δ, check reachable, fill `distance` (arc length), `time` (arc / 2.91), and `estimatedGoalPose` (goal x,y + yaw = start yaw + 2α).

**Done when:** tests reproduce your Day 12 hand-calculations through the `ControllerInterface*` pointer (not the concrete class).

---

## Day 18: `Ackerman::reachGoal`: the control loop

**Do** the loop (blocking in A1):
```
while distance to goal > tolerance:
    read odometry
    recompute α and δ (re-aiming each step corrects drift)
    send steering δ * STEERING_RATIO, throttle 0.1, brake 0
    accumulate distance travelled + time in motion
send brake = MAX_BRAKE, throttle 0
```
Note the steering command is the **steering wheel** angle: wheel angle δ × steering ratio.

**Done when:** against the mock, `reachGoal()` returns true for a reachable goal and false (without moving) for an unreachable one.

---

## Day 19: `Mission` + run in Gazebo

**Do**
- `Mission : public MissionInterface`, constructed with `std::vector<ControllerInterface*>`.
- `setGoals` stores goals; `runMission` gives each goal to the platform in turn and calls `reachGoal`.
- **No assumptions about platform order or count**; this is what makes it work for Quadcopter next week.
- Launch the A1 world and run your `main` with 2 goals, then 5 goals.

**Done when:** the Audi visits 5 goals in Gazebo and `main` prints distance travelled and time moving, close to your predictions.

---

## Day 20: Tests + documentation polish

**Do**
- Port or rewrite the three test files from the brief: `test_audi.cpp` (reachability, distance, time), `test_calculations.cpp` (maths), `test_mission.cpp` (multi-goal mission).
- Add a "five goals" mission test.
- Doxygen comments on every class/method, plus `index.dox` describing usage and expected output.

**Done when:** all tests green; Doxygen HTML builds with no undocumented-member warnings for your classes.

---

## Day 21: Buffer / review against the marking criteria

Go through the A1 *Assessment Criteria* table line by line and score yourself honestly:
- Default construction works for every class?
- I/O only in `main`?
- Common data/functions only in `Controller` (no duplication)?
- `Controller` can't be instantiated?
- `Mission` would work unchanged with a Quadcopter?

Fix the weakest two items.
