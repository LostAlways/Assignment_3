# PFMS Re-learn: 8 weeks, 1 hour a day

This is a self-paced plan for re-learning 41012 *Programming for Mechatronic Systems* from first principles and then rebuilding all three assignments:

| Assignment | Theme | What it really teaches |
|---|---|---|
| A1: Abstraction of Platform Classes | C++ OOP, the Ackerman (Audi R8) platform, pure pursuit | Classes, inheritance, polymorphism, interfaces, unit tests, Doxygen |
| A2: Threading, Synchronisation and Data Integrity | Ackerman + Quadcopter on missions, TSP goal ordering | `std::thread`, mutexes, condition variables, graphs and search |
| A3: Final Project (ROS 2) | Pick one: Search & Rescue / Track Racing / Grand Tour | ROS 2 nodes, topics, services, laser processing, rosbag unit tests |

**End goal:** a working A3 project (Track Racing is recommended, see below), built on reusable A1/A2 code, with unit tests and Doxygen docs.

---

## How each session works (60 minutes)

| Minutes | Activity |
|---|---|
| 0–5 | Re-read yesterday's notes and your "next step" line |
| 5–15 | **Learn**: read or watch the topic for today |
| 15–50 | **Build**: write the code for the exercise. Typing it beats reading it |
| 50–60 | **Check**: run the "Done when" test, commit, and write one "next step" line in `LOG.md` |

Rules that make 1 hour/day work:
- **Commit at the end of every session**, even if it is broken (`git commit -m "wip: day 12"`). Starting from a known point tomorrow is worth more than tidy history.
- **Stop at 60 minutes.** Write down where you were. Momentum comes from consistency, not long sessions.
- **Day 7 of each week is a buffer.** Use it to catch up, or to review. If you are ahead, do the ⭐ stretch tasks.
- If you are stuck for more than 20 minutes, write the question in `LOG.md`, skip to the next part, and come back on the buffer day.

---

## The 8-week map

| Week | Phase | File | Ends with |
|---|---|---|---|
| 1 | Environment + modern C++ refresh | [00-setup-and-cpp.md](00-setup-and-cpp.md) | Ubuntu + ROS 2 Humble + simulator running; CMake + GoogleTest project |
| 2 | OOP + vehicle maths | [01-oop-and-vehicle-maths.md](01-oop-and-vehicle-maths.md) | Pure-pursuit maths library with passing unit tests |
| 3 | **Assignment 1 rebuild** | [02-assignment-1.md](02-assignment-1.md) | Ackerman driving to goals in Gazebo via `Mission` |
| 4 | Concurrency fundamentals | [03-concurrency.md](03-concurrency.md) | Thread-safe, non-blocking controller with `run()`/`status()` |
| 5 | Graphs + **Assignment 2 rebuild** | [04-graphs-and-assignment-2.md](04-graphs-and-assignment-2.md) | Both platforms running concurrently; TSP ordering tested |
| 6 | ROS 2 fundamentals | [05-ros2-fundamentals.md](05-ros2-fundamentals.md) | Node with pub/sub, service, params, rosbag-driven gtest |
| 7–8 | **Assignment 3 project** | [06-assignment-3.md](06-assignment-3.md) | Full P/C solution, then D/HD |

Supporting material:
- [formulas.md](formulas.md): every equation you need in one place (pure pursuit, transforms, laser → world, quaternions).
- [LOG.md](LOG.md): your daily log (template included).
- [testing-without-the-simulator.md](testing-without-the-simulator.md): what's still public without university access, and how to test your C++ with a fake car before using Gazebo. Starter project in [examples/sim-free-starter/](examples/sim-free-starter/).
- **Progress tracker:** <https://claude.ai/artifact/1FUjtwYZMQ8yhr5L6dkGFQ> (source in `tracker.html`).

Each week is **6 working sessions + 1 buffer**. That is 48 planned hours plus 8 buffer hours across the 2 months.

---

## Which A3 project? Recommendation: Project 2, Track Racing

| | Search & Rescue (drone) | **Track Racing (Ackerman)** | Grand Tour (both) |
|---|---|---|---|
| Reuses your A1 Ackerman work | Low | **High** (pure pursuit) | Medium |
| Sensors | 360° laser | **180° laser only** | Laser + sonar, two robots |
| Hardest part | Detecting doors/people in 3D | Clustering cones, pairing sides | Coordinating 2 robots + TSP |
| Fit for an 8-week, 1 hr/day plan | OK | **Best** | Hard |

Track Racing builds directly on A1 (Ackerman steering) and A2 (goal following in a thread), and uses one sensor. Week 7–8 in `06-assignment-3.md` is written for Track Racing, with notes for switching to the other two. **Grand Tour is a good "month 3" follow-on** if you want more.

---

## Before you start: things you need

1. **A Linux machine running Ubuntu 22.04 (Jammy).** ROS 2 Humble, which the subject uses, targets 22.04. Options, best first:
   - Native install or dual boot (Gazebo needs CPU; this is the smoothest).
   - WSL2 on Windows 11 (works; see the `LIBGL_ALWAYS_SOFTWARE` tip in pfms-support FAQ).
   - A VM (works for weeks 1–5; Gazebo can be slow).
2. **The simulator packages** from <https://github.com/41012/pfms-support>.
   - The current `README` installs via an apt repo: `sudo apt install pfms-ros-integration ros-humble-pfms ros-dev-tools`.
   - The **`2024` branch** contains the exact version from your year, including `a3_racing.launch.py`, `a3_search.launch.py`, `a3_terrain.launch.py` and the `pipes` `.deb` packages. If the current apt packages don't include the A3 launch files, use the `2024` branch (instructions in its `INSTALLATION.md`).
3. **No university access?** The simulator and platform libraries are public. See [testing-without-the-simulator.md](testing-without-the-simulator.md) for what's missing and what to use instead.
4. **Your old skeleton code** (`a1_skeleton`, `2024a_a2_skeleton`, `a3_skeleton`) from your university GitHub repo, if you still have access. If not, that's fine: the plan has you write your own interfaces from the specs, which teaches more anyway. Where an interface is needed, the phase file gives a minimal version to write yourself.

> Note on launch names: the 2024 PDFs say `ros2 launch gazebo_tf a2.launch.py`. Newer pfms-support releases use `ros2 launch pfms a2.launch.py`. Run `ros2 pkg list | grep -E "pfms|gazebo_tf"` to see which you have.

---

## Progress checklist

Tick these off as you go.

- [ ] W1: Ubuntu 22.04 + ROS 2 Humble installed, `ros2 launch ... a1.launch.py` shows the Audi in Gazebo/RViz
- [ ] W1: CMake project with a library, an executable and GoogleTest tests
- [ ] W2: `Shape`-style abstract base class + polymorphism exercise
- [ ] W2: Pure-pursuit maths functions unit-tested against hand-calculated values
- [ ] W3: A1 `Controller` / `Ackerman` / `Mission` with tests passing, Doxygen generated
- [ ] W4: Producer/consumer with `condition_variable`; non-blocking `run()` + `status()`
- [ ] W5: Quadcopter added; BASIC mode running both platforms concurrently
- [ ] W5: TSP (brute force via `std::next_permutation`) in a separate library, unit-tested
- [ ] W6: ROS 2 node with subscriber, publisher, service, parameter; rosbag-driven gtest
- [ ] W7: A3 P/C: cone detection, goal-in-cones check, obstacle stop, steering to goals
- [ ] W8: A3 D/HD: road-centre goals from cone pairs, `_advanced:=true` parameter
- [ ] W8: Doxygen `index.dox` + README describing behaviour
- [ ] ⭐ Bonus: spline look-ahead goal for smoother driving
