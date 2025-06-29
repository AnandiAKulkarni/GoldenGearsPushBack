#include "vex.h"

extern const float skills_turn_settle_time = 150;
extern const float skills_turn_timeout = 2500;
extern const float skills_drive_settle_time = 150;
// AI Vision Color Descriptions
aivision::colordesc AISensor__BLUE(1, 31, 159, 192, 19, 0.42);
aivision::colordesc AISensor__RED(2, 238, 46, 43, 10, 0.15);
aivision::colordesc AISensor__YELLOW(3, 241, 186, 54, 10, 0.19);
// AI Vision Code Descriptions
vex::aivision AISensor(PORT14, AISensor__BLUE, AISensor__RED, AISensor__YELLOW);
bool blueRingDetected = false;
int noBlueRingTimes = 0;
int blueRingTimes = 0;
int blockCertaintyLevel = 0;


/**
 * Performs a fast turn to a specified angle using the chassis.
 * 
 * @param angle The target angle in degrees to which the chassis should turn.
 * 
 * The function uses predefined turn constants for voltage, settle time, 
 * and timeout to achieve a quick and efficient turn. Once the turn is 
 * complete, the chassis is stopped with a hold.
 */

void fast_turn(int angle) {
  //  chassis.turn_to_angle(angle);
  //  chassis.drive_stop(hold);
   chassis.turn_to_angle(angle,12,1,skills_turn_settle_time, skills_turn_timeout);
   chassis.drive_stop(hold);
}

int look_for_blue_blocks() {
  while (true) {
    // Get a snapshot of all Blue Color objects.
    AISensor.takeSnapshot(AISensor__BLUE);
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print(AISensor.objectCount);
    // Check to make sure an object was detected in the snapshot before starting intake.
    if (AISensor.objectCount > 0) {
      blueRingDetected = true;
      intake_block_in();
    }
    else
    {
      blueRingDetected = false;
      wait(2,seconds);
      intake_stop();
    }
    wait(5, msec);
  }
    return(0);
}

void AITest(){
  //Start a loop that is constantly looking for blue blocks
  while (true) {
    // Get a snapshot of all Blue Color objects.
    AISensor.takeSnapshot(AISensor__BLUE);
    // Check to make sure an object was detected before changing a class-wide field and starting the intake to pick it up.
    if (AISensor.objectCount > 0) {
      blueRingDetected = true;
      intake_block_in();
    }
    //If no object is detected, set the boolean to false 
    else
    {
      blueRingDetected = false;
    }
    //Give the brain and sensors a break, give the boolean time to update
    wait(5, msec);

    //If an object is NOT detected, increment a counter used to make sure there is no block, keep turning to keep scanning
    if(blueRingDetected == false){
      noBlueRingTimes += 1;
      turnRight();
      //Make it clear that we are NOT SURE if we see a block
      blockCertaintyLevel = 0;
      //If we haven't seen a block for a while, go to another location and reset the no block certainty counter to 0
      if(noBlueRingTimes > 500){
      intake_stop();
      chassis.drive_with_voltage(6,6);
      wait(0.8,seconds);
      chassis.drive_stop(coast);
      noBlueRingTimes = 0;
    }
    }
    //If an object IS detected, increment a counter used to make sure there is a block
    if(blueRingDetected == true){
      blueRingTimes += 1;
      blockCertaintyLevel += 1;
      //If we see a block for a while but can't get it, let it go.
      if(blueRingTimes > 300){
      chassis.drive_with_voltage(-8,-1);
      wait(0.6,seconds);
      chassis.drive_stop(coast);
      blueRingTimes = 0;
      }
      //THEN, if we are CERTAIN that there is a block, go for it with the intake
      if (blockCertaintyLevel > 22){
        intake_block_in();
        chassis.drive_with_voltage(6,6);
        wait(0.5,seconds);
        chassis.drive_stop(coast);
        //Once the block is picked up, set the certaintly level back to 0
        blockCertaintyLevel = 0;
      }
    }
    wait(5, msec);
  }
}

void drive_to_point_local(float xPosition, float yPosition, float velocity) {
  float currentX = GPSSensor.xPosition();
  float currentY = GPSSensor.yPosition();
  float distance = sqrt(pow(xPosition - currentX, 2) + pow(yPosition - currentY, 2));
  float dx = xPosition - currentX;
  float dy = yPosition - currentY;
  float angle = atan2(dy, dx);

  if (dx < 0 && dy > 0) {
    angle += M_PI;
  } else if (dx < 0 && dy < 0) {
    angle -= M_PI;
  }

  chassis.drive_with_voltage(velocity * cos(angle), velocity * sin(angle));
  while (distance > 1) {
    currentX = GPSSensor.xPosition();
    currentY = GPSSensor.yPosition();
    distance = sqrt(pow(xPosition - currentX, 2) + pow(yPosition - currentY, 2));
    dx = xPosition - currentX;
    dy = yPosition - currentY;
    angle = atan2(dy, dx);

    if (dx < 0 && dy > 0) {
      angle += M_PI;
    } else if (dx < 0 && dy < 0) {
      angle -= M_PI;
    }

    chassis.drive_with_voltage(velocity * cos(angle), velocity * sin(angle));
    wait(20, msec);
  }
  chassis.drive_stop(coast);
}

void intake_block_in(){
  Chain.setVelocity(90, percent);
  Chain.spin(reverse);
  BoxIntake.setVelocity(90, percent);
  BoxIntake.spin(forward);
  HighRoller.setVelocity(90, percent);
  HighRoller.spin(reverse);
}

void scoreInMiddle(){
  Chain.setVelocity(100, percent);
  Chain.spin(reverse);
  BoxIntake.setVelocity(50, percent);
  BoxIntake.spin(reverse);
  HighRoller.setVelocity(90, percent);
  HighRoller.spin(forward);
}

void scoreInLow(){
  Chain.setVelocity(90, percent);
  Chain.spin(forward);
  BoxIntake.setVelocity(90, percent);
  BoxIntake.spin(reverse);
}

void scoreInHigh(){
  Chain.setVelocity(100, percent);
  Chain.spin(reverse);
  BoxIntake.setVelocity(50, percent);
  BoxIntake.spin(reverse);
  HighRoller.setVelocity(90, percent);
  HighRoller.spin(reverse);
}

void intake_stop(){
  Chain.stop();
  BoxIntake.stop();
  HighRoller.stop();
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
  chassis.set_drive_constants(10, 1.5, 0, 10, 0);
  chassis.set_heading_constants(6, .4, 0, 1, 0);
  chassis.set_turn_constants(12, .4, .03, 3, 15);
  chassis.set_swing_constants(12, .3, .001, 2, 15);

  // Each exit condition set is in the form of (settle_error, settle_time, timeout).
  chassis.set_drive_exit_conditions(1.5, 300, 5000);
  chassis.set_turn_exit_conditions(1, 300, 3000);
  chassis.set_swing_exit_conditions(1, 300, 3000);
}

/**
 * Sets constants to be more effective for odom movements.
 * For functions like drive_to_point(), it's often better to have
 * a slower max_voltage and greater settle_error than you would otherwise.
 */

void odom_constants(){
  default_constants();
  chassis.heading_max_voltage = 10;
  chassis.drive_max_voltage = 8;
  chassis.drive_settle_error = 3;
  chassis.boomerang_lead = .5;
  chassis.drive_min_voltage = 0;
}
void turnRight(){
  LeftBackTop.spin(reverse,3,volt);
  LeftBackBottom.spin(reverse,3,volt);
  LeftFront.spin(reverse,3,volt);
  RightBackTop.spin(forward,3,volt);
  RightBackBottom.spin(forward,3,volt);
  RightFront.spin(forward,3,volt);
}

/**
 * Should end in the same place it began, but the second movement
 * will be curved while the first is straight.
 */


void tank_odom_test(){
  odom_constants();
  chassis.set_coordinates(0, 0, 0);
  chassis.turn_to_point(3, 3);
  chassis.drive_to_point(3,3);
  chassis.drive_to_point(0,0);
  chassis.turn_to_angle(0);
}
