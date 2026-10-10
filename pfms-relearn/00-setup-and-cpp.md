# Week 1: Environment + modern C++ refresh

**Goal for the week:** a working toolchain (so later weeks are about learning, not fighting installs), and your C++ fingers warmed up.

Create a practice repo (or a folder in this one) called `pfms-practice/` with one subfolder per week.

---

## Day 1: Ubuntu, ROS 2 Humble, simulator

**Learn**
- ROS 2 Humble install guide (Ubuntu deb packages): <https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debs.html>
- pfms-support README and FAQ: <https://github.com/41012/pfms-support> (also look at the `2024` branch)

**Do**
1. Install ROS 2 Humble (`ros-humble-desktop`) and `ros-dev-tools`.
2. Add `source /opt/ros/humble/setup.bash` to `~/.bashrc`.
3. Install the pfms packages (apt route from the README, or the `2024` branch route).
4. Launch the A1 world: `ros2 launch pfms a1.launch.py` (or `gazebo_tf` on the 2024 version).

**Done when:** you can see the Audi in RViz/Gazebo, and in a second terminal `ros2 topic list` shows topics like `/orange/odom`.

> Installs can overrun. If this takes 2 sessions, use Day 7 as the buffer. Do not skip it: everything after week 2 needs it.

---

## Day 2: Build tooling: CMake, libraries, executables

**Learn**
- What `add_library`, `add_executable`, `target_link_libraries`, `target_include_directories` do.
- The out-of-source build pattern: `mkdir build && cd build && cmake .. && make`.

**Do:** create `week1/geometry/` with:
```
CMakeLists.txt
include/geometry.h      // declarations
src/geometry.cpp        // definitions
src/main.cpp            // uses the library
```
Put in it: `double distance(double x1, double y1, double x2, double y2);` and `double normaliseAngle(double a);` (wrap to −π…π).

**Done when:** `./main` prints the distance between (0,0) and (3,4) as `5`, and `normaliseAngle(3*M_PI/2)` prints `-1.5708`.

---

## Day 3: GoogleTest

**Learn**
- `TEST(Suite, Name)`, `EXPECT_EQ`, `EXPECT_NEAR`, `ASSERT_*` vs `EXPECT_*`.
- How to wire gtest into CMake: `find_package(GTest REQUIRED)`, `gtest_discover_tests` or plain `add_test`.

**Do:** add `test/test_geometry.cpp` with at least 6 tests: zero distance, symmetric distance, negative coordinates, angle already in range, angle > π, angle < −π.

**Done when:** `ctest` (or `./test_geometry`) is all green, and you have deliberately broken one function once to watch a test go red.

---

## Day 4: Memory, references, pointers, smart pointers

**Learn**
- Pass by value vs `const &` vs pointer. When each is right.
- `std::unique_ptr`, `std::shared_ptr`, `std::make_shared`. Why A1's FAQ warns about a `shared_ptr` going out of scope.
- RAII: destructors release resources.

**Do:** write a `Logger` class that prints in its constructor and destructor. Create one on the stack, one with `new`/`delete`, one with `unique_ptr`, one shared between two `shared_ptr`s. Predict the print order on paper **before** you run it.

**Done when:** your prediction matched, or you can explain why it didn't.

---

## Day 5: STL containers and algorithms

**Learn**
- `std::vector`, `std::deque`, `std::map`, `std::set`, `std::pair`.
- `std::sort` with a lambda, `std::min_element`, `std::accumulate`, `std::next_permutation`.

**Do:** with `struct Point { double x, y; };`:
1. Make a `vector<Point>` of 6 points.
2. Sort them by distance from origin (lambda).
3. Find the closest point to (5,5) using `min_element`.
4. Compute total path length visiting them in order (`accumulate` or loop).
5. Print every permutation of `{0,1,2,3}` using `next_permutation` (you'll reuse this for TSP in week 5).

**Done when:** 24 permutations print, and your sorted order is correct.

---

## Day 6: Doxygen + Git hygiene

**Learn**
- Doxygen comment style: `/** @brief ... @param ... @return ... */`.
- `doxygen -g` then `doxygen Doxyfile`. A `mainpage`/`index.dox` file.

**Do:** document every function in `geometry.h`. Write an `index.dox` with a `@mainpage` describing what the library does and how to run the tests. Generate HTML and open `html/index.html`.

**Done when:** the generated page shows your mainpage and every function has a description.

---

## Day 7: Buffer / review

- Finish anything left from Days 1–6.
- Self-check (answer in `LOG.md` without looking it up):
  1. What's the difference between `EXPECT_NEAR` and `EXPECT_EQ`, and why do we always use `NEAR` for doubles?
  2. When would you return `const std::vector<Point>&` instead of `std::vector<Point>`?
  3. What does `target_link_libraries(main geometry)` do?
- ⭐ Stretch: add a `cmake` option `BUILD_TESTS` (A2 used `set(BUILD_TESTS OFF)`) that switches tests on/off.
