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

bool runauton=true;
Train rt = Train(onetile,r1,r2);
Train lt = Train(onetile,l1,l2);
Lift lift = Lift(1000, lift1, lift2);
Intake intake = Intake(il,ir);
bool sp = false;

int team_color = 255;

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


void autonmode(int b) {

	if(b==2) {
		pros::lcd::set_text(3,"AUTON OFF");
		runauton=false;
	} else {
		pros::lcd::set_text(3,"AUTON ON");
		runauton=true;
	}
}
void on_button_1() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		autonmode(1);
	} else {
		autonmode(1);
	}
}

void on_button_2() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		autonmode(2);
	} else {
		autonmode(2);
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
	vs.clear_led();
	vs.set_signature(1, &BLUE_SIG);
	vs.set_signature(2, &RED_SIG);
	pros::lcd::register_btn1_cb(on_center_button);
	pros::lcd::register_btn0_cb(on_button_1);
	pros::lcd::register_btn2_cb(on_button_2);
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
	int facing = (int(gyro.get_rotation()) % 360);
	if(facing < 0) {
		facing += 360;
	}
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
	lt.rpm(lrpm);
	rt.rpm(rrpm);
}

void movebasetile(double tile, int speed) {

	lt.moveTile(tile, speed);
	rt.moveTile(-tile, speed);

	while(!(rt.stopped() && lt.stopped())) {
		pros::delay(3);
	}
	lt.rpm(0);
	rt.rpm(0);
	rt.resetEncoders();
	lt.resetEncoders();
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
	lt.rpm(speeds[0]);
	rt.rpm(speeds[1]);
}

void constgyro() {
	while(true) {
		//pros::lcd::set_text(4,std::to_string(getgyro()));
	}
}

/*bool keepturn(int deg, int err) {
	if(getgyro() > deg+err || getgyro()<deg-err) {
		return true;
	}
	return false;
}
*/

void stopbase() {
	lt.stop();
	rt.stop();
}

bool turn(int tt, int turndeg, int speed, int err) {
	int ct = tt-getgyro();
	int ms = int(float(speed)*0.2);
	if(ct<0) {ct+=360;}

	if(ct<=180) {
		lt.rpm(int(float(ct)/float(turndeg)*(speed)+ms));
		rt.rpm(-int(float(ct)/float(turndeg)*(speed)+ms));
	} else {
		lt.rpm(-int(float(ct)/float(turndeg)*(speed)+ms));
		rt.rpm(int(float(ct)/float(turndeg)*(speed)+ms));
	}

	//pros::lcd::set_text(1,"LEFT ENCODER: " + std::to_string(lt.getPos()));

	if(abs(ct)<err) {
		rt.resetEncoders();
		lt.resetEncoders();
		stopbase();
		return false;

	} else {
		return true;
	}

}


bool drive(float tiles, int speed, int err) {
	int tt = tiles*onetile;
	int ct = abs(tt-lt.getPos());
	int ms = int(float(speed)*0.2);


	//pros::lcd::set_text(1,"LEFT ENCODER: " + std::to_string(lt.getPos()));


	if(tt>0) {
	lt.rpm(int(float(ct)/float(tt)*(speed)+ms));
	rt.rpm(int(float(ct)/float(tt)*(speed)+ms));
} else {
	lt.rpm(int(float(ct)/float(tt)*(speed)-ms));
	rt.rpm(int(float(ct)/float(tt)*(speed)-ms));
}
	if(abs(ct)<=err) {
		rt.resetEncoders();
		lt.resetEncoders();
		stopbase();
		return false;
	} else {
		return true;
	}
}



void autonomous() {
if(runauton) {
	pros::delay(3000);

	lift.unload(600);
	pros::delay(1000);
	intake.spin(200);
	pros::delay(500);
	lift.spin(0);
	while(drive(1.5,150,50)) {pros::delay(1);}
	pros::delay(250);

	lift.spin(0);
	intake.spin(0);

	while(turn(135,135,150,4)) {pros::delay(1);}
	pros::delay(250);

	while(drive(1.26,150,50)) {pros::delay(1);}
	pros::delay(250);


	/*

	THEORETICAL SCORING OPERATIONS

	*/

	lift.spin(600);
	pros::delay(700);
	intake.spin(200);
	pros::delay(500);
	intake.spin(-200);
	lift.spin(-600);


	while(drive(-1.1,150,50)) {pros::delay(1);}
	intake.spin(0);
	lift.spin(0);
	pros::delay(250);

	while(turn(270, 135, 50, 4)) {pros::delay(1);}
	pros::delay(250);

	intake.spin(200);
	while(drive(1.2,100,50)) {pros::delay(1);}
	pros::delay(250);

	intake.spin(0);

	while(turn(195, 75, 100, 4)) {pros::delay(1);}
	pros::delay(250);

	while(drive(1,150,50)) {pros::delay(1);}
	pros::delay(250);


	lift.spin(600);
	pros::delay(700);
	intake.spin(200);
	pros::delay(900);
	intake.spin(-200);
	lift.spin(-600);


	while(drive(-0.25,150,50)) {pros::delay(1);}
	lift.spin(0);
	pros::delay(250);

	while(turn(270,90,50,4)) {pros::delay(1);}
	pros::delay(250);

	intake.spin(200);
	while(drive(2.2,150,50)) {pros::delay(1);}
	lift.spin(600);
	pros::delay(250);
	lift.spin(0);

	while(drive(-0.5,150,50)) {pros::delay(1);}
	pros::delay(250);

	while(turn(235,35,50,4)) {pros::delay(1);}
	pros::delay(250);

	while(drive(.8,150,50)) {pros::delay(1);}
	pros::delay(250);

	lift.spin(600);
	pros::delay(1000);
	intake.spin(200);
	pros::delay(500);
	intake.spin(-200);
	lift.spin(-600);


	while(drive(-1.41,150,50)) {pros::delay(1);}
	pros::delay(250);





/*
lift.spin(-600);
intake.spin(200);
pros::delay(1500);
lift.spin(0);
pros::delay(500);
rt.rpm(100);
lt.rpm(100);
pros::delay(500);
rt.rpm(0);
lt.rpm(0);
intake.spin(200);
lift.spin(600);
pros::delay(1500);
lift.spin(0);
intake.spin(-200);
lt.rpm(-200);
rt.rpm(-200);
pros::delay(500);
lt.rpm(0);
rt.rpm(0);
intake.spin(0);
*/
} else {

	lt.rpm(50);
	rt.rpm(-50);
	while(getgyro() < 70) {
		pros::delay(3);
	}
	lt.rpm(0);
	rt.rpm(0);
	pros::delay(20000);

}

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

		pros::lcd::set_text(1, "LEFT TRAIN" + std::to_string(lt.getPos()));
		pros::lcd::set_text(2, "RIGHT TRAIN" + std::to_string(rt.getPos()));

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
		lt.rpm(lefty+leftx);
		rt.rpm(lefty-leftx);

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
			lt.drift();
			rt.drift();
		}

		if(lt.isdrift()) {
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
		lt.resetEncoders();
		rt.resetEncoders();
	}
	}

}
