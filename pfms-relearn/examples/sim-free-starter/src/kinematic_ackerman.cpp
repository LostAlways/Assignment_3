#include "kinematic_ackerman.h"

#include <algorithm>
#include <cmath>

double KinematicAckerman::wheelAngle() const {
  // Same as the Gazebo plugin: divide by the steering ratio, then clamp.
  return std::clamp(cmd_.steering / kSteeringRatio, -kMaxSteerAngle, kMaxSteerAngle);
}

void KinematicAckerman::step(double dt) {
  speed_ = (cmd_.brake > 0.0) ? 0.0 : std::max(0.0, cmd_.throttle) * kSpeedPerThrottle;

  pose_.x += speed_ * std::cos(pose_.yaw) * dt;
  pose_.y += speed_ * std::sin(pose_.yaw) * dt;
  pose_.yaw += speed_ / kWheelbase * std::tan(wheelAngle()) * dt;
  pose_.yaw = std::atan2(std::sin(pose_.yaw), std::cos(pose_.yaw));  // wrap to -pi..pi
}
