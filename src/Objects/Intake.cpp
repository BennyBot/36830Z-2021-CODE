#include "main.h"

class Intake {
public:
  pros::Motor m1;
  pros::Motor m2;
  int alternator = 1;
  Intake(pros::Motor m, pros::Motor mm): m1(m), m2(mm) {};

  void spin(int rpm) {
    m1.move_velocity(alternator*rpm);
    m2.move_velocity(rpm);
  }


};
