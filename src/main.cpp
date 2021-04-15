#include "Objects/Train.cpp"
#include "Objects/Dispense.cpp"
#include "Objects/Lift.cpp"
#include "Objects/Intake.cpp"


pros::Motor r1(4, pros::E_MOTOR_GEARSET_18,true);
pros::Motor r2(3, pros::E_MOTOR_GEARSET_18);
pros::Motor l1(2, pros::E_MOTOR_GEARSET_18);
pros::Motor l2(1, pros::E_MOTOR_GEARSET_18,true);
pros::Motor lift1(5,pros::E_MOTOR_GEARSET_06);
pros::Motor lift2(8, pros::E_MOTOR_GEARSET_06,true);
pros::Motor il(6,pros::E_MOTOR_GEARSET_18);
pros::Motor ir(7,pros::E_MOTOR_GEARSET_18,true);
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Imu gyro(21);
pros::Vision vs(10);



int onetile = 1719;
int commandPause = 250; //In ms

bool runauton=true;
Train rightTrain = Train(onetile,r1,r2);
Train leftTrain = Train(onetile,l1,l2);
Lift lift = Lift(1000, lift1, lift2);
Intake intake = Intake(il,ir);
bool sp = false;

int team_color = 2;


int turnSpeed = 90;
int driveSpeed = 200;




pros::vision_signature_s_t BLUE_SIG = pros::Vision::signature_from_utility(1, -3319, -2383, -2851, 10045, 14479, 12262, 2.600, 0);
pros::vision_signature_s_t RED_SIG = pros::Vision::signature_from_utility(2, 7835, 10789, 9312, -925, -111, -518, 3.000, 0);
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {

	} else {
		pros::lcd::clear_line(2);
	}
}



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();

	gyro.reset();
	while(gyro.is_calibrating()) {
		pros::delay(2);
	}
	vs.clear_led();
	vs.set_signature(1, &BLUE_SIG);
	vs.set_signature(2, &RED_SIG);
	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


int getgyro() {

	/*int facing = (int(gyro.get_rotation()) % 360);
	if(facing < 0) {
		facing += 360;
	}*/
	int facing = gyro.get_heading();
	return facing;
}
/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void movebaserpm(int lrpm, int rrpm) {
	leftTrain.rpm(lrpm);
	rightTrain.rpm(rrpm);
}

void movebasetile(double tile, int speed) {

	leftTrain.moveTile(tile, speed);
	rightTrain.moveTile(-tile, speed);

	while(!(rightTrain.stopped() && leftTrain.stopped())) {
		pros::delay(3);
	}
	leftTrain.rpm(0);
	rightTrain.rpm(0);
	rightTrain.resetEncoders();
	leftTrain.resetEncoders();
}


void rotatebase(int speed, int deg) {

	if(deg > 180) {
		movebaserpm(-speed,speed);
	} else {
		movebaserpm(speed,-speed);
	}

	while(getgyro() < deg) {
		pros::delay(3);
		//pros::lcd::set_text(4, std::to_string(getgyro()));
	}
	movebaserpm(0,0);
}

void rotatebaserpm(int *speeds) {
	leftTrain.rpm(speeds[0]);
	rightTrain.rpm(speeds[1]);
}

void constgyro() {
	while(true) {
		//pros::lcd::set_text(4,std::to_string(getgyro()));
	}
}

/*bool keepturn(int deg, int error) {
	if(getgyro() > deg+error || getgyro()<deg-error) {
		return true;
	}
	return false;
}
*/

void stopbase() {
	leftTrain.stop();
	rightTrain.stop();
}

void pTurn(int turnTo, int speed=turnSpeed) {

	int gyrovalue = getgyro();
	float diff = (turnTo-gyrovalue + 180) % 360 - 180;
	int currentTurn = (diff < -180 ? diff + 360 : diff);
	int minimumSpeed = 10;
	int turnDeg = abs(currentTurn);
	int currentSpeed = float(currentTurn)/float(turnDeg)*speed*2;
	currentSpeed = (currentSpeed > speed ? speed : currentTurn);
			do {
				do {
					gyrovalue = getgyro();
					pros::lcd::set_text(3, "GYRO VALUE : " + std::to_string(gyrovalue));
					diff = (turnTo-gyrovalue + 180) % 360 - 180;
					currentTurn = (diff < -180 ? diff+360 : diff);
					//currentSpeed = int((abs(currentTurn)>25 ? speed : float(currentTurn))/float(turnDeg)*float(speed));
					currentSpeed = int(float(currentTurn)/float(turnDeg)*float(speed));

					if(abs(currentSpeed) < minimumSpeed){
						currentSpeed = (currentSpeed < 0 ? -minimumSpeed : minimumSpeed);
					}

						leftTrain.rpm(currentSpeed);
						rightTrain.rpm(-1*currentSpeed);

					pros::delay(5);
				} while(turnTo!=gyrovalue);
				stopbase();
				pros::delay(100);
			}
			while(turnTo!=gyrovalue);
			//while(getgyro()!=turnTo);
	stopbase();
	rightTrain.resetEncoders();
	leftTrain.resetEncoders();
	pros::delay(commandPause);
}

void pDrive(float tiles, int speed=driveSpeed){
	int turnTo = tiles*onetile; //This is the amount of tiles that are desired, converted into encoder ticks
	int minimumSpeed = 40;
	int leftSpeed = speed;
	int currentTurn = turnTo-leftTrain.getPos();
	int error = 0;
	float Kp = 0.5; //This is the tuning constant for the P loop

	while(abs(currentTurn)>=5) {
		pros::lcd::set_text(3, "GYRO VALUE : " + std::to_string(getgyro()));
		currentTurn = turnTo-leftTrain.getPos();
		error = leftTrain.getPos()-rightTrain.getPos();

		leftSpeed = int(float(currentTurn)/float(abs(turnTo))*(speed));
		if (abs(leftSpeed)<minimumSpeed){
			leftSpeed = (leftSpeed < 0 ? -minimumSpeed : minimumSpeed);
		}

		int rightSpeed = leftSpeed + int(float(error)*Kp);

		leftTrain.rpm(leftSpeed);
		rightTrain.rpm(rightSpeed);

		pros::delay(5);
	}
	stopbase();
	rightTrain.resetEncoders();
	leftTrain.resetEncoders();

	pros::delay(commandPause);
}

int get_ball_color() {
	pros::vision_object_s_t obj = vs.get_by_size(0);
	if(obj.width>50) {
		return obj.signature;
	}
	return 255;
}

void score_red_ball(int speed) {
	lift.spin(-600);
	pros::delay(100);
	lift.spin(speed);
	pros::delay(1550);
	lift.spin(0);
}

void intake_ball() {
	lift.spin(600);
	pros::delay(250);
	intake.spin(0);
	lift.spin(0);
}
void tower_auton(int ispeed, int lspeed) {
	//until intake blue ball
	intake.spin(ispeed);
	lift.mult=1;
	while(get_ball_color()!=1) {
		lift.autospin(lspeed);
	}

	lift.mult=-1;
	while(get_ball_color()!=2) {
		lift.autospin(lspeed);
	}

	lift.spin(-lspeed);
	intake.spin(-ispeed);

	pros::delay(500);
	intake.spin(0);
	lift.spin(0);
}

void ejectintakes() {

	lift.unload(600);
	pros::delay(500);
	lift.spin(0);
	/*
	int multiplier =1;
	do {
		lift2.move_velocity(-600*multiplier);
		intake.spin(200*multiplier);
		pros::delay(100);
		multiplier = (multiplier-1)^2;
	} while(get_ball_color()!=2);

	*/
}

void reset_gyro() {
	stopbase();
	rightTrain.resetEncoders();
	leftTrain.resetEncoders();
	pros::lcd::set_text(3, "CALIBRATION IN PROGRESS");
	gyro.reset();
	while(gyro.is_calibrating()) {
		pros::delay(1);
	}
	pros::lcd::set_text(3, "CALIBRATION DONE");
	pros::delay(50);
}

void autonomous() {


	if(leftTrain.isdrift()) {
		leftTrain.drift();
	}
	if(rightTrain.isdrift()) {
		rightTrain.drift();
	}

	reset_gyro();
	ejectintakes();
	pros::delay(3000);
	//ejectintakes();
	//Get first ball
	intake.spin(200);
	pDrive(1.65);
	intake_ball();

	//Turn and go to goal
	pTurn(135);
	pDrive(1.2);
	score_red_ball(600);

	//Backup from goal
	pDrive(-1.24);

	pTurn(263);
	intake.spin(200);
	pDrive(1.4);
	intake_ball();

	pTurn(180);

	pDrive(0.92);

	score_red_ball(600);

	pDrive(-0.8,100);

	//pTurn(263);
	pTurn(230,75);
	//pTurn(235,50);
	intake.spin(200);
	pDrive(2.3);
	intake_ball();
	/*rightTrain.rpm(150);
	pros::delay(250);
	rightTrain.rpm(0);
	pros::delay(50);
	*/
/*
	pDrive(-0.5,driveSpeed);

	pTurn(235,100);

	pDrive(0.7,driveSpeed);
*/
	score_red_ball(600);

	//pDrive(-0.2,100);

	pTurn(350,170);


	leftTrain.rpm(-150);
	rightTrain.rpm(-150);
	pros::delay(1000);
	stopbase();
	pros::delay(50);
	reset_gyro();
	pros::delay(2000);

	intake.spin(200);
	pDrive(2.7,driveSpeed);
	intake_ball();

	pTurn(270,160);

	pDrive(0.1,100);

	score_red_ball(600);

	pDrive(-0.5,150);

	pTurn(350);

	intake.spin(200);
	pDrive(2.2,300);
	intake_ball();

leftTrain.rpm(-200);
pros::delay(550);
leftTrain.rpm(100);
rightTrain.rpm(100);
score_red_ball(600);
leftTrain.rpm(-200);
rightTrain.rpm(-200);
/*
	pDrive(-1.2);

	pTurn(330);

	pDrive(1.3);

	score_red_ball(600);

	pDrive(-1);

	pTurn(90);

	intake.spin(200);
	pDrive(1.5);
	intake_ball();

	pTurn(180);

	pDrive(0.7);

	pDrive(-0.3,100);

	score_red_ball(600);

	pDrive(-0.6);
*/

}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

bool holding = false;
bool holdr1 = false;
bool holdr2 = false;
bool holdl1 = false;
bool holdl2 = false;

int prevlmu = 0;
int prevlmd = 0;
std::string hold = "DRIFTY";
int liftmodeup = 0;
int liftmodedown = 0;
int inmodedraw = 0;
int inmodeleave = 0;

void opcontrol() {

	/*pros::lcd::register_btn1_cb(on_center_button);
	pros::lcd::register_btn0_cb(on_button_1);
	pros::lcd::register_btn2_cb(on_button_2);
*/

	//initialize();
	int prevball = 255;
	while (true) {


		pros::vision_object_s_t obj = vs.get_by_size(0);
		int ballcol = obj.signature;
		int ballsize = 0;

		if(team_color == 255) {
			team_color = ballcol;
		}


		if(obj.width>50) {
			ballsize= 300;
		} else {
			ballcol=255;
		}

		pros::lcd::set_text(1, "LEFT TRAIN" + std::to_string(leftTrain.getPos()));
		pros::lcd::set_text(2, "RIGHT TRAIN" + std::to_string(rightTrain.getPos()));
		pros::lcd::set_text(3, "GYRO: " + std::to_string(getgyro()));
		pros::lcd::set_text(4, "TEAM COLOR: " + std::to_string(team_color));
		pros::lcd::set_text(5, "COLOR: " + std::to_string(ballcol));

		//pros::lcd::set_text(3, std::to_string(ballsize));
		//pros::lcd::set_text(2, std::to_string(ballcol));

		if(ballcol != 255) {
			prevball=ballcol;
		}
		if(prevball !=255 && prevball!=team_color) {
			lift.mult=-1;
		} else if(prevball!=255 && prevball==team_color) {
			lift.mult=1;
		}

		int facing = (int(gyro.get_rotation()) % 360);
		if(facing < 0) {
			facing += 360;
		}
		//pros::lcd::set_text(1, std::to_string(facing));
		int lefty = master.get_analog(ANALOG_LEFT_Y)*2;
		int leftx = master.get_analog(ANALOG_LEFT_X)*1.5;


/*
		if(lefty>0) {
			lefty = (lefty*lefty)/100;
		} else {
			lefty = -(lefty*lefty)/100;
		}
		if(leftx>0) {
			leftx = (leftx*leftx)/500;
		} else {
			leftx = -(leftx*leftx)/50;
		}
*/
		leftTrain.rpm(lefty+leftx);
		rightTrain.rpm(lefty-leftx);

		if(master.get_digital(DIGITAL_R1)) {
			holdr1 = true;
		} else if(!master.get_digital(DIGITAL_R1) && holdr1) {
			holdr1 = false;
			liftmodeup -= 1;
			liftmodeup = liftmodeup*liftmodeup;
			liftmodedown = 0;
		}

		if(master.get_digital(DIGITAL_R2)) {
			holdr2 = true;
		} else if(!master.get_digital(DIGITAL_R2) && holdr2) {
			holdr2 = false;
			liftmodedown -= 1;
			liftmodedown = liftmodedown*liftmodedown;
			liftmodeup = 0;
		}

		if(master.get_digital(DIGITAL_L1)) {
			holdl1 = true;
		} else if(!master.get_digital(DIGITAL_L1) && holdl1) {
			holdl1 = false;
			inmodedraw -= 1;
			inmodedraw = inmodedraw*inmodedraw;
			inmodeleave = 0;
		}

		if(master.get_digital(DIGITAL_L2)) {
			holdl2 = true;
		} else if(!master.get_digital(DIGITAL_L2) && holdl2) {
			holdl2 = false;
			inmodeleave -= 1;
			inmodeleave = inmodeleave*inmodeleave;
			inmodedraw = 0;
		}

		if(master.get_digital(DIGITAL_A)) {
			holding = true;
		} else if(!master.get_digital(DIGITAL_A) && holding) {
			holding = false;
			leftTrain.drift();
			rightTrain.drift();
		}

		if(leftTrain.isdrift()) {
			pros::lcd::set_text(6,"Drifty");
			hold = "DRIFTY";
		} else {
			pros::lcd::set_text(6,"Not Drifty");
			hold = "NOT DRIFTY";
		}


		/*
		Toggle mode
		if(liftmodeup == 1) {
			lift.spin(400);
		} else if(liftmodedown == 1) {
			lift.spin(-400);
		} else {
			lift.spin(0);
		}
*/


		if(master.get_digital(DIGITAL_R1) || master.get_digital(DIGITAL_R2)) {
			lift.autospin(600);
		} else if(master.get_digital(DIGITAL_UP)) {
			lift.spin(600);
		} else if(master.get_digital(DIGITAL_DOWN)) {
			lift.spin(-600);
		} else {
			lift.spin(0);
		}

		if(master.get_digital(DIGITAL_L1)) {
			intake.spin(200);
		} else if(master.get_digital(DIGITAL_L2)) {
			intake.spin(-200);
		} else {
			intake.spin(0);
		}

		pros::delay(1);


		if(master.get_digital(DIGITAL_Y)) {
		leftTrain.resetEncoders();
		rightTrain.resetEncoders();
		}
		/*
		if(master.get_digital(DIGITAL_X)) {
			tower_auton(200,600);
		}
		if(master.get_digital(DIGITAL_LEFT)) {
			ejectintakes();
		}
		*/
	}

}
