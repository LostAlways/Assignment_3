#ifndef KINEMATIC_ACKERMAN_H
#define KINEMATIC_ACKERMAN_H

#include "platform_io.h"

/**
 * @brief A fake Audi R8 that moves with the bicycle model.
 *
 * Time only moves when you call step(), so tests run in microseconds and
 * give the same answer every time. Speed changes instantly (no inertia),
 * so expect the real simulator to be a little slower to start and stop.
 */
class KinematicAckerman : public AckermanIO {
 public:
  KinematicAckerman() = default;
  explicit KinematicAckerman(Pose start) : pose_(start) {}

  Pose pose() override { return pose_; }
  void send(const AckermanCommand& cmd) override { cmd_ = cmd; }

  /// Advance the fake world by dt seconds.
  void step(double dt);

  double speed() const { return speed_; }
  double wheelAngle() const;  ///< front wheel angle actually applied [rad]

  static constexpr double kWheelbase = 2.65;
  static constexpr double kSteeringRatio = 17.3;
  static constexpr double kLockToLockRevs = 3.2;
  static constexpr double kMaxSteerAngle = 3.14159265358979323846 * kLockToLockRevs / kSteeringRatio;
  static constexpr double kSpeedPerThrottle = 2.91 / 0.1;  ///< 0.1 throttle -> 2.91 m/s

 private:
  Pose pose_;
  AckermanCommand cmd_;
  double speed_{0.0};
};

#endif  // KINEMATIC_ACKERMAN_H
