#ifndef PLATFORM_IO_H
#define PLATFORM_IO_H

/**
 * @file platform_io.h
 * @brief The only thing your controller code is allowed to talk to.
 *
 * Your Ackerman / Quadcopter / Mission classes depend on this interface,
 * never on Gazebo, ROS or PfmsConnector directly. Then:
 *  - in unit tests you hand them a KinematicAckerman (fake, instant, repeatable)
 *  - in Gazebo you hand them a class that wraps PfmsConnector or ROS topics
 * and the controller code does not change at all.
 */

/// Robot pose in the world frame. x forward, y left, yaw CCW in radians (-pi..pi).
struct Pose {
  double x{0.0};
  double y{0.0};
  double yaw{0.0};
};

/// Ackerman command, same meaning as the simulator's topics.
struct AckermanCommand {
  double steering{0.0};  ///< steering WHEEL angle [rad] (= wheel angle * 17.3)
  double throttle{0.0};  ///< 0..1 (0.1 gives ~2.91 m/s)
  double brake{0.0};     ///< 0..8000 Nm
};

class AckermanIO {
 public:
  virtual ~AckermanIO() = default;
  virtual Pose pose() = 0;                           ///< latest odometry
  virtual void send(const AckermanCommand& cmd) = 0; ///< apply a command
};

#endif  // PLATFORM_IO_H
