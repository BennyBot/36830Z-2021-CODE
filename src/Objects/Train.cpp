#include "main.h"

//This function is applicable to both sides of the robot, IE it can be applied to both the left and the right train.

class Train {
public:
  pros::Motor backMotor;
  pros::Motor frontMotor;
  int onetile;
  bool dr = true;
  Train(int ot, pros::Motor front, pros::Motor back): onetile(ot),  frontMotor(front), backMotor(back) {
    frontMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    backMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
  };

  void rpm(int speedRPM) {
      frontMotor.move_velocity(speedRPM);
      backMotor.move_velocity(speedRPM);
  }

  bool isdrift() {
    if(dr) {
      return true;
    } else {
      return false;
    }
  }
  void stop() { //stop moving the train
      frontMotor.move_velocity(0);
      backMotor.move_velocity(0);
  }

  void drift() {
    if (dr) {
      backMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
      frontMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    } else {
      backMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
      frontMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    }
    dr=!dr;
  }

  void moveTile(float distance, int velocity) {
      float toMove = distance*((float)onetile);
      frontMotor.move_relative(toMove,velocity);
      backMotor.move_relative(toMove,velocity);
  }

  void rotateTick(int tick, int velocity) {
    frontMotor.move_relative(tick,velocity);
    backMotor.move_relative(tick,velocity);
  }

  int getPos() {
    return (int)frontMotor.get_position();
  }

  void resetEncoders() {
    frontMotor.tare_position();
    backMotor.tare_position();
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
    return (frontMotor.is_stopped() && backMotor.is_stopped());
  }
};
