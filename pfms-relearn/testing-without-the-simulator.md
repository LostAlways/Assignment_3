# Working outside university + testing without the simulator

## 1. Can you replicate all of this without university access?

Mostly, yes. The simulator and the platform libraries are public. What you lose is the code and material that lived on Canvas and in your private university repos.

| Thing | Available? | Where / what to do instead |
|---|---|---|
| Gazebo worlds + launch files (A1, A2, A3 racing / search / terrain) | ✅ Public | [`41012/pfms-support`](https://github.com/41012/pfms-support): apt install from the current README, or the `2024` branch (has `a3_racing.launch.py`, `a3_search.launch.py`, `a3_terrain.launch.py`) |
| `pipes` library: `PfmsConnector` (send commands / read odometry, laser, sonar), `pfms_types.h` | ✅ Public | `2024` branch, `packages/pipes_3.0.5-humble_amd64.deb` |
| `Audi` helper library (`audi.h`: `checkOriginToDestination`, `computeSteering`) | ✅ Public | Same `.deb`. Use it as an **answer key**: test your own pure-pursuit maths against it |
| Assignment briefs | ✅ You have them | |
| Skeleton code (`ControllerInterface`, `MissionInterface`, a3_skeleton) | ❌ | Write your own interfaces from the briefs (Day 15 gives a starting shape). Arguably better practice |
| Supplied unit tests, a1_snippets, quizzes, weekly tutorials | ❌ | Write your own tests (that's the skill); use the official ROS 2 Humble tutorials for weeks 6+ |
| Supplied rosbags | ❌ | Record your own with `ros2 bag record` in the simulator (Day 41) |
| A marker / VIVA | ❌ | Use the *Assessment Criteria* tables as a self-review checklist at the end of each assignment week |

Notes:
- If the current apt packages behave differently from your 2024 briefs (for example, A1 is now Audi + Husky), install from the **`2024` branch** instead so the worlds match your assignments.
- The `pipes` `.deb` is for x86-64 (`amd64`); there's also an `arm64` build.
- `PfmsConnector` is a ROS 2 node under the hood, so the simulator must be running when you use it.

---

## 2. How to test before going near the simulator

The rule that makes this possible: **your controller code never talks to Gazebo directly.** It talks to a small interface. In tests you plug in a fake; in the simulator you plug in a thin wrapper around `PfmsConnector` (A1/A2) or ROS topics (A3).

```
                    your code (Ackerman, Quadcopter, Mission, cone detection ...)
                                         │ depends only on
                                         ▼
                              AckermanIO  (interface: pose(), send(cmd))
                     ┌───────────────────┼─────────────────────┐
                     ▼                   ▼                     ▼
          KinematicAckerman      GazeboAckerman           RosAckerman
          fake, for unit tests   wraps PfmsConnector      topics, for A3
          (instant, repeatable)  (A1/A2 in Gazebo)        (week 6+)
```

This is called *dependency injection*: you hand the platform in through the constructor instead of creating it inside the class. It's also what the A1 brief means by building a "mock (fake) platform".

### The testing ladder

Climb one rung at a time. Only move up when the rung below is green.

| Rung | What you test | Needs | Speed | Plan days |
|---|---|---|---|---|
| 1. Pure functions | Maths: angle wrapping, α, δ, arc length, reachability, laser → world, TSP cost | gtest only | microseconds | 2–5, 11–12, 31–32 |
| 2. Classes against a fake | `Ackerman::reachGoal`, `Mission`, progress %, goal ordering | gtest + `KinematicAckerman` | milliseconds | 13, 15–20, 25–27, 29–30 |
| 3. Recorded or synthetic sensor data | Cone detection, goal-inside-track, obstacle check | gtest + fake laser scans, later rosbags | milliseconds | 38, 41, 44–46 |
| 4. Simulator | Tuning, timing, the whole system | Gazebo + ROS | real time | 19, 30, 47, 52 |

If something fails on rung 4, write a rung 1–3 test that reproduces it, fix it there, then go back to the simulator.

### Starter project (already working)

[`examples/sim-free-starter/`](examples/sim-free-starter/) is a small CMake project with:
- `include/platform_io.h`: the `AckermanIO` interface and `Pose` / `AckermanCommand` types
- `KinematicAckerman`: a fake Audi using the bicycle model with the real Audi constants. Steering works like the Gazebo plugin (divided by 17.3, clamped to 0.581 rad); 0.1 throttle = 2.91 m/s. Time only moves when you call `step(dt)`
- `drive_demo`: prints the car's path as CSV so you can plot it
- 5 passing gtests showing the pattern (straight line, circle of radius L/tan δ, steering clamp, brake)

It needs no ROS:
```bash
sudo apt install build-essential cmake libgtest-dev
cd pfms-relearn/examples/sim-free-starter
cmake -S . -B build && cmake --build build
ctest --test-dir build --output-on-failure
./build/drive_demo > path.csv
```

How to use it with the plan:
- **Day 13** asks you to write a mock platform. Write your own first, then compare with the starter (or just use it if time is short).
- **Weeks 3–5**: give your `Ackerman` a constructor that takes an `AckermanIO&` (or `std::shared_ptr<AckermanIO>`). Tests pass a `KinematicAckerman`; `main` passes your Gazebo wrapper.
- Add a `KinematicQuadcopter` yourself in week 5. Simpler still: `x += vx·dt` after rotating body velocities into the world frame (see [formulas.md](formulas.md)).

### Example: testing `reachGoal` with no simulator

```cpp
TEST(Ackerman, ReachesGoalAheadAndLeft) {
  KinematicAckerman fake;                  // starts at (0,0), facing +x
  Ackerman audi(fake);                     // your class, given the fake
  ASSERT_TRUE(audi.setGoal({10.0, 5.0}));

  // Your control loop calls fake.send(...) each cycle. In tests, step the
  // fake between cycles instead of waiting for real time to pass.
  for (int i = 0; i < 60000 && !audi.goalReached(); ++i) {
    audi.controlStep();                    // one cycle of your loop
    fake.step(0.001);
  }
  EXPECT_TRUE(audi.goalReached());
  EXPECT_NEAR(audi.distanceTravelled(), expectedArcLength, 0.5);
}
```

Splitting the loop into a `controlStep()` you can call once is a good design change in itself: in A1 `reachGoal()` just calls it in a loop, and in A2 the thread calls it in a loop.

### Testing threads (week 4–5) without the simulator

- Run the fake in its own thread that calls `step(0.01)` every 10 ms of real time. Now `run()` / `status()` behave just like they will in Gazebo.
- Speed it up 10× (step 0.1 s every 10 ms) so mission tests finish in seconds.
- Build with `-fsanitize=thread` once a week to catch data races, and `-fsanitize=address,undefined` for memory bugs:
  `cmake -S . -B build-tsan -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"`

### Testing laser processing (week 6–8) without the simulator

Before you have rosbags, generate fake scans:
1. Put cones (circles, radius ~0.15 m) at known world positions.
2. For each laser beam angle, ray-cast from the laser position and return the nearest hit, or `inf`.
3. Feed that scan + pose into your cone-detection function and check it returns the cones you placed.

This catches most frame and sign mistakes (laser offset, yaw direction) before you ever launch Gazebo. Then record real rosbags and add tests from them as well.

### Habits that keep code testable

- **No `std::cout` inside classes.** Return values; print in `main`. (Also an A1 marking item.)
- **Inject time.** Pass `dt`, or a clock object, rather than calling `std::chrono::now()` deep inside your maths.
- **Keep ROS out of your logic.** In A3, the node converts ROS messages to your own structs and calls library functions. The library has no `#include <rclcpp/...>`.
- **One class, one job.** If a test needs a lot of setup, the class is probably doing too much.
