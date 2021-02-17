#include "main.h"

class Train {
public:
  pros::Motor bm;
  pros::Motor fm;
  int onetile;
  bool dr = true;
  Train(int ot, pros::Motor front, pros::Motor back): onetile(ot),  fm(front), bm(back) {
    fm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    bm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  };

  void rpm(int speedRPM) {
      fm.move_velocity(speedRPM);
      bm.move_velocity(speedRPM);
  }

  bool isdrift() {
    if(dr) {
      return true;
    } else {
      return false;
    }
  }
  void stop() { //stop moving the train
      fm.move_velocity(0);
      bm.move_velocity(0);
  }

  void drift() {
    if (dr) {
      bm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
      fm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    } else {
      bm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
      fm.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    }
    dr=!dr;
  }



  void moveTile(float distance, int velocity) {
      float toMove = distance*((float)onetile);
      fm.move_relative(toMove,velocity);
      bm.move_relative(toMove,velocity);
  }

  void rotateTick(int tick, int velocity) {
    fm.move_relative(tick,velocity);
    bm.move_relative(tick,velocity);
  }

  int getPos() {
    return (int)fm.get_position();
  }

  void resetEncoders() {
    fm.tare_position();
    bm.tare_position();
  }

  bool movedTiles(double tiles, int line) {
    float toMove = tiles*((float)onetile);
    int h = this->getPos();

    pros::lcd::set_text(line, std::to_string(toMove) + " : " + std::to_string(h));
    if(h<toMove) {
      return false;
    }
    return true;
  }
  bool stopped() {
    return (fm.is_stopped() && bm.is_stopped());
  }
};
