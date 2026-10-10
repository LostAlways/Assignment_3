// Example tests. Copy this pattern for your own classes: build the fake,
// drive it with your controller, check where it ended up.
#include <gtest/gtest.h>

#include <cmath>

#include "kinematic_ackerman.h"

namespace {
void run(KinematicAckerman& car, double seconds, double dt = 0.001) {
  for (double t = 0.0; t < seconds; t += dt) car.step(dt);
}
}  // namespace

TEST(KinematicAckerman, StartsAtOriginByDefault) {
  KinematicAckerman car;
  EXPECT_DOUBLE_EQ(car.pose().x, 0.0);
  EXPECT_DOUBLE_EQ(car.pose().y, 0.0);
  EXPECT_DOUBLE_EQ(car.pose().yaw, 0.0);
}

TEST(KinematicAckerman, DefaultThrottleDrivesStraightAt291) {
  KinematicAckerman car;
  car.send({0.0, 0.1, 0.0});
  run(car, 10.0);
  EXPECT_NEAR(car.pose().x, 29.1, 0.05);
  EXPECT_NEAR(car.pose().y, 0.0, 1e-9);
}

TEST(KinematicAckerman, ConstantSteeringFollowsCircleOfRadiusLOverTanDelta) {
  KinematicAckerman car;
  const double delta = 0.2;  // wheel angle
  car.send({delta * KinematicAckerman::kSteeringRatio, 0.1, 0.0});
  const double R = KinematicAckerman::kWheelbase / std::tan(delta);

  // Half a lap: arc length pi*R at 2.91 m/s. Should end 2R to the left.
  run(car, M_PI * R / 2.91);
  EXPECT_NEAR(car.pose().x, 0.0, 0.05);
  EXPECT_NEAR(car.pose().y, 2.0 * R, 0.05);
  EXPECT_NEAR(std::fabs(car.pose().yaw), M_PI, 0.01);
}

TEST(KinematicAckerman, SteeringIsClampedToMaxSteerAngle) {
  KinematicAckerman car;
  car.send({100.0, 0.1, 0.0});  // far past full lock
  EXPECT_NEAR(car.wheelAngle(), KinematicAckerman::kMaxSteerAngle, 1e-12);
  EXPECT_NEAR(KinematicAckerman::kMaxSteerAngle, 0.581, 1e-3);
}

TEST(KinematicAckerman, BrakeStopsTheCar) {
  KinematicAckerman car;
  car.send({0.0, 0.1, 0.0});
  run(car, 1.0);
  car.send({0.0, 0.0, 8000.0});
  const double x = car.pose().x;
  run(car, 1.0);
  EXPECT_DOUBLE_EQ(car.pose().x, x);
}
