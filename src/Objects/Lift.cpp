#include "main.h"
class Lift {
public:
  pros::Motor m1;
  pros::Motor m2;
  int mult=1;

  int bottotop;

  Lift(int o, pros::Motor mot, pros::Motor m): bottotop(o), m1(mot), m2(m) {
    m1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    m2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  };

  void autospin(int rpm) {
    m1.move_velocity(rpm*mult);
    m2.move_velocity(abs(rpm));
  }

  void spin(int rpm) {
    m1.move_velocity(rpm);
    m2.move_velocity(rpm);
  }
  void unload(int rpm) {
    m1.move_velocity(-abs(rpm));
    m2.move_velocity(-abs(rpm));
  }
  void output(int rpm) {
    m1.move_relative(bottotop, rpm);
    m2.move_relative(bottotop, rpm);
  }

  void ball_amount(int amount, int rpm) {
    m1.move_relative((bottotop/3)*amount, rpm);
    m2.move_relative((bottotop/3)*amount, rpm);
  }
};
