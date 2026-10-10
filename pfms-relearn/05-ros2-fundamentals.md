# Week 6: ROS 2 fundamentals

**Goal for the week:** move from "my library + my main" (A1/A2) to "a ROS 2 package with nodes" (A3). By Day 41 you'll have a node that subscribes to odometry and laser, publishes commands and markers, answers a service, reads parameters, and is unit-tested from a rosbag.

Official tutorials (Humble): <https://docs.ros.org/en/humble/Tutorials.html>. Do the C++ ("rclcpp") versions.

Key shift from A2: **you don't own `main`'s flow any more.** Data arrives in callbacks. Keep heavy logic in a plain C++ library (testable without ROS), and keep the node a thin wrapper.

---

## Day 36: Workspace, packages, CLI tools

**Learn:** `colcon build --symlink-install`, `ros2 pkg create --build-type ament_cmake`, sourcing `install/setup.bash`.

**Do**
- Create `~/ros2_ws/src/pfms_practice` with `ros2 pkg create`.
- With the A3 racing world launched (`a3_racing.launch.py`), explore: `ros2 topic list`, `ros2 topic echo /orange/odom --once`, `ros2 topic hz /orange/laserscan`, `ros2 interface show sensor_msgs/msg/LaserScan`.

**Done when:** you can say, from the CLI alone, the laser's `angle_min`, `angle_max`, `angle_increment`, `range_max`, and the odometry rate.

---

## Day 37: Publisher + subscriber

**Do:** a node class (`class Racer : public rclcpp::Node`) that:
- subscribes to `/orange/odom` and stores the latest pose (mutex-protected; you'll read it from another thread later),
- publishes `std_msgs/msg/Float64` to `/orange/throttle_cmd` (0.1) on a timer.

**Done when:** the Audi creeps forward and your node logs its x,y,yaw at 1 Hz using `RCLCPP_INFO_THROTTLE`.

---

## Day 38: Laser scans → points in the world

**Do**
- Subscribe to `/orange/laserscan`.
- Convert each valid range to (x, y) in the laser frame, then into the world frame using the car's odometry and the laser offset (≈ 1.5 m along the car's x-axis; confirm the sign with data). See [formulas.md](formulas.md).
- Put the maths in your **library** (e.g. `laserprocessing.cpp`), not the node.

**Done when:** a cone that's 10 m ahead in Gazebo prints a world position within ~0.3 m of where Gazebo shows it.

---

## Day 39: RViz markers + services + parameters

**Do**
- Publish a `visualization_msgs/msg/MarkerArray` on `/visualisation_marker`: one CYLINDER (r=0.2, h=0.5) per detected laser point cluster (namespace `cones`). Use unique `id`s and set `lifetime` or reuse ids so markers don't pile up.
- Service server on `/orange/mission` of type `std_srvs/srv/SetBool`: `data=true` starts, `false` stops; respond with `success` and a `message`.
- A parameter `advanced` (bool, default false) read at start-up: `ros2 run pfms_practice racer --ros-args -p advanced:=true`.
  - The A3 brief writes this in ROS 1 style (`_advanced:=true`). In ROS 2 it is `--ros-args -p advanced:=true`.

**Done when:** `ros2 service call /orange/mission std_srvs/srv/SetBool "{data: true}"` toggles behaviour, and RViz shows cylinders on the cones.

---

## Day 40: Threads in a ROS node

**Learn:** callbacks run in the executor; a long loop inside a callback blocks everything else. Options: a dedicated control thread (the a3_skeleton approach), a timer, or a `MultiThreadedExecutor` with callback groups.

**Do:** move your control loop into its own `std::thread` (started in the constructor, joined in the destructor) that runs at a fixed rate and reads the latest odometry/laser under a mutex. Callbacks only store data.

**Done when:** Ctrl+C shuts the node down cleanly and `ros2 topic hz` shows your commands at a steady rate.

---

## Day 41: rosbags + unit tests

**Learn:** `ros2 bag record`, `ros2 bag info`, reading a bag in C++ with `rosbag2_cpp::Reader` and deserialising messages. Unit tests with `ament_add_gtest`.

**Do**
1. Record a short bag of `/orange/odom` + `/orange/laserscan` with the car parked in front of some cones.
2. A gtest that opens the bag, takes the first odom + laser message, calls your **library** cone-detection function, and checks the number/positions of cones against what you read off RViz.

**Done when:** `colcon test --packages-select pfms_practice && colcon test-result --verbose` is green.

---

## Day 42: Buffer / review

- Self-check:
  1. Why should detection logic live in a library and not in the node class?
  2. What happens if your control loop runs inside the laser callback?
  3. How do you stop markers accumulating forever in RViz (the "data does not accumulate endlessly" criterion)?
- Read your `a3_skeleton` README if you have it, and compare its structure with what you built.
