// Drives the fake Audi with a fixed steering command for 10 s and prints CSV.
// Plot it:  ./drive_demo > path.csv   then open path.csv in a spreadsheet,
// or: python3 -c "import pandas as p;d=p.read_csv('path.csv');d.plot('x','y').figure.savefig('path.png')"
#include <iostream>

#include "kinematic_ackerman.h"

int main() {
  KinematicAckerman car;
  car.send({/*steering*/ 0.2 * KinematicAckerman::kSteeringRatio, /*throttle*/ 0.1, /*brake*/ 0.0});

  const double dt = 0.01;
  std::cout << "t,x,y,yaw\n";
  for (int i = 0; i <= 1000; ++i) {
    if (i % 10 == 0) {
      Pose p = car.pose();
      std::cout << i * dt << ',' << p.x << ',' << p.y << ',' << p.yaw << '\n';
    }
    car.step(dt);
  }
  return 0;
}
