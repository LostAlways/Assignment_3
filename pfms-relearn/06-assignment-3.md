# Weeks 7–8: Assignment 3: Track Racing (Project 2)

**Goal:** a ROS 2 package that drives the Audi around the cone track: detect cones with the laser, check goals are inside the track, stop for obstacles, steer to goals, and (D/HD) find the road centre itself.

Re-read the A3 PDF *Project 2: Track Racing*, *Assessment Criteria*, *FAQ*. Launch: `ros2 launch pfms a3_racing.launch.py` (or `gazebo_tf`), add `gui:=true` to see Gazebo.

### Interface you must meet (from the brief)

| Direction | Topic / service | Type |
|---|---|---|
| Sub | `/orange/odom` | `nav_msgs/msg/Odometry` |
| Sub | `/orange/laserscan` | `sensor_msgs/msg/LaserScan` |
| Sub | `/orange/goals` | `geometry_msgs/msg/PoseArray` |
| Pub | `/orange/brake_cmd`, `/orange/steering_cmd`, `/orange/throttle_cmd` | `std_msgs/msg/Float64` |
| Pub | `/visualisation_marker` | `MarkerArray` (ns `cones`: CYLINDER r 0.2 h 0.5; ns `road`: CUBE 0.5) |
| Pub | `/orange/cones` | `geometry_msgs/msg/PoseArray` |
| Srv | `/orange/mission` | `std_srvs/srv/SetBool` (start/stop; `success` = a cone is visible; `message` = % complete) |

### Suggested structure
```
a3_racing/
  include/ src/
    laserprocessing.{h,cpp}   // pure C++: scan + pose -> cones, obstacle check   (unit-tested)
    trackgeometry.{h,cpp}     // pure C++: cone pairing, goal-in-track, road centres (unit-tested)
    ackerman_control.{h,cpp}  // pure C++: pure pursuit from week 2/3             (unit-tested)
    racer_node.{h,cpp}        // ROS wrapper: subs/pubs/service/params + control thread
    main.cpp
  test/ (gtests reading rosbags)
  data/ (your small rosbags)
  doc/index.dox
```

---

## Week 7: Pass/Credit

### Day 43: Package skeleton + interface
Create the package with every topic, the service and the `advanced` parameter wired up, doing nothing yet. Node idle until `/orange/mission` `data:true`; stops (brake) on `data:false`.
**Done when:** `ros2 node info` lists exactly the interface in the table above.

### Day 44: Cone detection
Laser points → world (Day 38) → **cluster**: consecutive valid readings closer than ~0.3 m belong to one object; cluster centre = mean. Filter by cluster size/width so only cone-sized objects count.
**Done when:** `/orange/cones` and the `cones` markers line up with the cones in RViz while you drive manually.

### Day 45: Is the goal inside the track?
Use the brief's hint: cones come in pairs across the track ~8 m apart (use a tolerance), and use at least two cones on each side.
- Split cones into left/right using the sign of their y in the car frame.
- Pair a left cone with the closest right cone (distance ≈ 8 m ± tolerance).
- Two consecutive pairs make a quadrilateral; the goal is "in" if it's inside one (point-in-polygon or cross-product side tests; see [formulas.md](formulas.md)).
**Done when:** a library unit test from a rosbag says a goal on the road is inside and one beyond the cones is outside.

### Day 46: Obstacle in front
Anything in a corridor directly ahead (e.g. |y| < 1 m, 0 < x < 8 m in the car frame) that is **not** a cone-sized cluster is an obstacle (they test with a fire truck). Stop: throttle 0, brake max.
**Done when:** unit test from a bag with something in front returns true; a bag with only cones returns false.

### Day 47: Control to goals
Control thread: take the next goal from `/orange/goals`, check it's inside the cones (otherwise **stop immediately and abandon**), compute pure pursuit δ (your week-2 code), publish steering = δ × steering ratio, throttle, brake. Advance when within tolerance. Report % complete in the service response.
**Done when:** publishing a few goals on `/orange/goals` makes the car drive them and stop at the end.

### Day 48: Unit tests + docs for P/C
Required tests: goal-within-cones and obstacle-in-front, each from a rosbag. Doxygen comments on every class/method, `index.dox` describing how to launch, call the service, and what you'll see.
**Done when:** `colcon test` green, Doxygen builds.

### Day 49: Buffer / full P/C run-through
Drive a full lap with supplied goals. Note every failure in `LOG.md` and fix the worst.

---

## Week 8: Distinction/High Distinction

### Day 50: Road centre from cone pairs
With `advanced:=true`, compute goals yourself: the midpoint of each left/right cone pair is a road-centre point. Publish them as `road` CUBE markers.
**Done when:** road-centre markers sit in the middle of the track ahead of the car.

### Day 51: Goal management
Merge your detected road centres with goals from `/orange/goals`: keep them ordered along the direction of travel, drop ones behind the car, don't re-add duplicates (within ~1 m). If **no cone pairs** are visible, stop and abandon.
**Done when:** the car keeps generating its own goals and drives further than the supplied goals alone would take it.

### Day 52: Full lap + tuning
Tune look-ahead distance, throttle and goal tolerance for a smooth lap. Keep values configurable as **parameters** (no hard-coded values is a marked criterion).
**Done when:** one full lap with `advanced:=true` and no manual help.

### Day 53: D/HD unit tests
Add the third test: detecting additional goals (road centres) from a rosbag. Check data structures: bounded containers, nothing grows forever.
**Done when:** all three tests green.

### Day 54: Documentation + self-VIVA
Finish `index.dox` (behaviour, how detection/pairing works, threading design, parameters). Then answer out loud, as if in the 10-minute VIVA:
- Why is your detection in a library?
- What does each mutex protect?
- What happens if the laser stops publishing?
- How would you add a second car?

### Day 55: ⭐ Bonus: spline look-ahead
Fit a spline (e.g. Catmull-Rom or cubic) through the upcoming road-centre goals and steer toward a point a few metres ahead **on the spline**, recomputed every control cycle, with look-ahead distance depending on curvature. Cite your source for the justification (e.g. Coulter 1992, *Implementation of the Pure Pursuit Path Tracking Algorithm*, or Snider 2009, the reference in the A1 brief). Add a unit test.

### Day 56: Wrap-up
Score yourself against the A3 *Assessment Criteria* table. Write a short `REFLECTION.md`: what was easy, what was hard, what you'd do next.

---

## Switching projects

**Project 1: Search & Rescue (drone).** Same weeks 7–8 rhythm. Replace cone detection with: wall-line detection from the 360° laser, door-gap detection (2 m openings in 12 × 8 m walls), and person detection (narrow cluster 1.5–1.9 m tall: change altitude to "slice" with the laser). The obstruction check is a ray/segment test between the drone and the goal against laser points. D/HD adds TSP (week 5 library!) with a cost that routes around the building.

**Project 3: Grand Tour (drone + Audi).** Drone flies the path first; ground height = drone altitude − sonar range; gradient between samples must be < 3%. Emit a waypoint every 1 m, mark "danger" where the gradient fails, then drive the Audi along valid waypoints. D/HD reuses your week-5 TSP to choose the shortest valid route. Good **month 3** project once Track Racing is done.
