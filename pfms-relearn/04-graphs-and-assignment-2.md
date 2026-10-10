# Week 5: Graphs, TSP + Assignment 2 rebuild

**Goal for the week:** rebuild A2 BASIC and ADVANCED modes: Ackerman + Quadcopter running concurrently through goals, with optimal goal ordering from a graph search in its **own library**.

Re-read the A2 PDF: *Specifics*, *Mission Planning & Controller Behaviour*, *Assessment Criteria*. SUPER mode is listed as ⭐ stretch.

---

## Day 29: Quadcopter platform

**Learn:** the Quadcopter controls are turn left/right, move left/right, up/down, forward/back (all "left/up/forward positive"). It must **TAKEOFF** first (see `a1_snippets/command_uav.cpp` if you have it). Keep height ≈ 2 m.

**Do**
- `Quadcopter : public Controller`. Distance = straight line, time = distance / speed.
- Control: rotate the world-frame error into the drone's body frame (see [formulas.md](formulas.md) "World → body"), then command forward/left velocities scaled to your speed. This is a P controller.
- Reuse your week-4 `run()`/`status()` pattern; check how much can live in `Controller` so it isn't duplicated.

**Done when:** in the A2 world (`a2.launch.py`), the drone takes off and reaches 3 goals using `run()` + `status()`.

---

## Day 30: BASIC mode: both platforms together

**Do**
- `Mission::setGoals(goals, platformType)` called twice (once per platform), `run()` non-blocking, `status()` = combined % complete.
- `main` (no threads in `main`!): read two goal files, create both platforms, create mission, set objective BASIC, `run()`, poll `status()` every 200 ms.

**Done when:** both robots move at the same time in Gazebo and the program terminates at 100%.

---

## Day 31: Graphs in C++

**Learn**
- Adjacency matrix (`vector<vector<double>>`) vs adjacency list (`map<int, vector<pair<int,double>>>`). For ≤ 10 goals a matrix is simplest.
- The edge weight must come from **the platform**: arc length for Ackerman (and it depends on the heading you arrive with), straight-line for Quadcopter. Reuse `checkOriginToDestination` for weights.

**Do:** build a `Graph` class in a separate library (`tsp/`) that takes a list of points and a cost function (`std::function<double(Point, Point)>`) and builds the matrix.

**Done when:** a unit test with 4 points on a square gives the expected matrix for a Euclidean cost.

---

## Day 32: TSP by brute force

**Learn:** with n goals there are n! orderings (5 goals = 120, fine). `std::next_permutation` over a sorted `vector<int>` walks them all.

**Do**
- `std::vector<int> bestOrder(const Graph&, Point start)`: try every permutation, sum costs from the start position, keep the minimum.
- Test it on points where you know the answer (points on a line given out of order; corners of a square).

**Done when:** tests pass, and the library has **no** dependency on Mission or the platforms (that's the "graph search separated into a library" marking item).

---

## Day 33: ADVANCED mode

**Do**
- Mission with objective ADVANCED: compute the order per platform with the TSP library, then run as BASIC.
- Implement `getPlatformGoalAssociation()` returning `vector<pair<int,int>>` (goal index, platform index).
- Ackerman caveat: arc cost depends on arrival heading, so compute the cost of a whole permutation by chaining `checkOriginToDestination` with the estimated end pose of the previous leg, not a fixed matrix.

**Done when:** your own unit tests ("TSP for Ackerman", "TSP for Quadcopter") check the ordering function alone; the mission visits goals in that order in Gazebo.

---

## Day 34: Tests, docs, design review

**Do**
- Tests mirroring the brief: reach goals via controller status, via mission status, mission with both, TSP Ackerman, TSP Quadcopter.
- `index.dox`: describe what `main` does when run, how the search works, and the threading design (which class owns which thread, what each mutex protects).
- Check the *Modularity* criteria: no repeated code, separate threads for platforms and mission, works with an extra Ackerman or Quadcopter, graph search in its own library.

**Done when:** all green, and you could explain your threading diagram in a 10-minute VIVA.

---

## Day 35: Buffer / review

- Self-check:
  1. Why can't a single fixed distance matrix be fully correct for the Ackerman?
  2. 10 goals → 10! = 3.6 million permutations. What would you do for 15 goals? (Look up: nearest neighbour heuristic, 2-opt, Held–Karp DP.)
  3. Where would a deadlock come from in your Mission/Platform design?
- ⭐ SUPER mode stretch: send the drone over the 3×5 obstacle lattice, use its laser to find obstacle centres (`getObstacles()`), and generate Ackerman goals through the gaps from (−80, 70) to (80, −70). Unit-test `getObstacles()`.
