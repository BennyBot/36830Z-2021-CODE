#include "main.h"

class Dispense {
public:
  pros::Motor mm;
  int one;
  
  Dispense(int ot, pros::Motor m): one(ot),  mm(m){
    m.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  };

  void rpm(int speedRPM) {
      mm.move_velocity(speedRPM);
  }


  void stop() { //stop moving the train
      mm.move_velocity(0);
  }


  void dispense(float distance, int velocity) {
      mm.move_relative(one,velocity);
  }

  void rotateTick(int tick, int velocity) {
    mm.move_relative(tick,velocity);

  }

  int getPos() {
    return (int)(mm.get_position());
  }

  void resetEncoders() {
    mm.tare_position();
  }

  int checkMoving() {
    if(mm.is_stopped()) {
      return false;
    }
    return true;
  }
};
