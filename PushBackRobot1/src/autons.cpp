#include "vex.h"

extern const float skills_turn_settle_time = 150;
extern const float skills_turn_timeout = 2500;
extern const float skills_drive_settle_time = 150;
// AI Vision Color Descriptions
aivision::colordesc AIVision__RED(1, 242, 90, 107, 10, 0.2);
aivision::colordesc AIVision__BLUE(3, 53, 190, 229, 30, 0.2);
// AI Vision Code Descriptions
aivision::codedesc AIVision__RED_BLUE(1, AIVision__RED, AIVision__BLUE);
vex::aivision AIVision(PORT5, AIVision__RED, AIVision__BLUE, AIVision__RED_BLUE, aivision::ALL_AIOBJS);


void fast_turn(int angle) {
  //  chassis.turn_to_angle(angle);
  //  chassis.drive_stop(hold);
   chassis.turn_to_angle(angle,12,1,skills_turn_settle_time, skills_turn_timeout);
   chassis.drive_stop(hold);

}
//For small, imprecise distances
void drive_short_distance(double short_distance){
// chassis.drive_distance(short_distance);
chassis.drive_distance(short_distance,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1000);
//used to be 11 out of 12, changed to 12
chassis.drive_stop(hold);
}

//For long, more precise distances
void drive_long_distance(double long_distance){
chassis.drive_distance(long_distance,chassis.get_absolute_heading(),9,12,1.5, skills_drive_settle_time, 5000);
chassis.drive_stop(hold);
}

/**
 * Resets the constants for auton movement.
 * Modify these to change the default behavior of functions like
 * drive_distance(). For explanations of the difference between
 * drive, heading, turning, and swinging, as well as the PID and
 * exit conditions, check the docs.
 */

void default_constants(){
  // Each constant set is in the form of (maxVoltage, kP, kI, kD, startI).  
  chassis.set_drive_constants(10, 1.5, 2, 10, 0); //  chassis.set_drive_constants(10, 1.5, 0, 10, 0);
  // 0.06 KI - works on stright but not on slanted field
  // 2 KI - works better - 0.5 deviationc inside on down slope 
  // 2 KI - works better - 0.5 deviationc on up slope 

  chassis.set_heading_constants(6, .4, 0.03, 1, 0); //chassis.set_heading_constants(6, .4, 0, 1, 0);
  chassis.set_turn_constants(12, .4, 0.03, 3.95, 15);
  chassis.set_swing_constants(12, .3, .001, 2, 15);

  // Each exit condition set is in the form of (settle_error, settle_time, timeout).
  chassis.set_drive_exit_conditions(1.5, 300, 2000);
  chassis.set_turn_exit_conditions(1, 300, 3000);
  chassis.set_swing_exit_conditions(1, 300, 3000);
  
  // odom_constants();
  // chassis.set_coordinates(0, 0, 0);
  

  Brain.Screen.printAt(5, 20, "%d", RotationSensor.value());
  Brain.Screen.printAt(5, 40, "Intake temp, chain temp:");
  Brain.Screen.printAt(5, 60, "%d", IntakeMotor.temperature());
  Brain.Screen.printAt(5, 80, "%d", ChainMotor.temperature());
  Brain.Screen.printAt(5, 100, "Drive temp: LB, LM, LF, RB, RM, RF");
  Brain.Screen.printAt(5, 120, "%d", LeftBack.temperature());
  Brain.Screen.printAt(5, 140, "%d", LeftMiddle.temperature());
  Brain.Screen.printAt(5, 160, "%d", LeftFront.temperature());
  Brain.Screen.printAt(5, 180, "%d", RightBack.temperature());
  Brain.Screen.printAt(5, 200, "%d", RightMiddle.temperature());
  Brain.Screen.printAt(5, 220, "%d", RightFront.temperature());
}

/**
 * Sets constants to be more effective for odom movements.
 * For functions like drive_to_point(), it's often better to have
 * a slower max_voltage and greater settle_error than you would otherwise.
 */

void odom_constants(){
  chassis.heading_max_voltage = 10;
  chassis.drive_max_voltage = 8;
  chassis.drive_settle_error = 3;
  chassis.boomerang_lead = .5;
  chassis.drive_min_voltage = 0;
}
void turnRight(){
  LeftBack.spin(reverse,3,volt);
  LeftMiddle.spin(reverse,3,volt);
  LeftFront.spin(reverse,3,volt);
  RightBack.spin(forward,3,volt);
  RightMiddle.spin(forward,3,volt);
  RightFront.spin(forward,3,volt);
}




/**
 * Should end in the same place it began, but the second movement
 * will be curved while the first is straight.
 */

void tank_odom_test(){
  default_constants();
  chassis.set_coordinates(0, 0, 0);
  chassis.turn_to_point(24, 24);
  chassis.drive_to_point(24,24);
  chassis.drive_to_point(0,0);
  chassis.turn_to_angle(0);
}

/**
 * Drives in a square while making a full turn in the process. Should
 * end where it started.
 */

void holonomic_odom_test(){
  default_constants();
  chassis.set_coordinates(0, 0, 0);
  chassis.holonomic_drive_to_pose(0, 18, 90);
  chassis.holonomic_drive_to_pose(18, 0, 180);
  chassis.holonomic_drive_to_pose(0, 18, 270);
  chassis.holonomic_drive_to_pose(0, 0, 0);
}