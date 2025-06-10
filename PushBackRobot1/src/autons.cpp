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
bool blueRingDetected = false;
int noBlueRingTimes = 0;
int blueRingTimes = 0;
int ringCertaintyLevel = 0;
bool isHookOut = false;
bool isSweeperDown = false;
bool isHangUp = false;
bool isManualSkillCorner1Running = false;
task manual_skill_corner_task = NULL;

void arm_to_rest(){
  ArmMotor.setStopping(coast);
  ArmMotor.spin(reverse);

  while (true){
    if (RotationSensor.value() > 24900) 
    {
      ArmMotor.stop(coast);
      break;
   }   
  }
}

int look_for_blue_rings() {
  while (true) {
    // Get a snapshot of all Blue Color objects.
    AISensor.takeSnapshot(AIVision__BLUE);
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print(AISensor.objectCount);
    // Check to make sure an object was detected in the snapshot before starting intake.
    if (AISensor.objectCount > 0) {
      blueRingDetected = true;
      intake_ring_in();
      chain_ring_up();
    }
    else
    {
      blueRingDetected = false;
      wait(2,seconds);
      intake_stop();
      chain_stop();
    }
    wait(5, msec);
  }
    return(0);
}

bool has_mogo() {
    ColorSensor.setLightPower(100, percent);
    for(int i = 0;i<100;i++ ) {
      wait(15, msec);
      color DetectedColor = ColorSensor.color();
      if ((DetectedColor == color::green) || (DetectedColor == color::yellow)) {
        Controller.Screen.clearScreen();
        Controller.Screen.print("detect green" );
        return(true);
      }
      else {
        Controller.Screen.print("no green %d", DetectedColor.hue());
        break;
      }
    } 
  return(false);
}
int hook_out_on_yellow() {
  int returnValue = 0;
  for (int i = 0;i<=5;i++)
  {
    ColorSensor.setLightPower(100, percent);
    color DetectedColor = ColorSensor.color();
    if ((DetectedColor.hue() < 120) && (DetectedColor.hue() > 40)) {
      //hook_out();
      returnValue = 1;
      break;
    }
    //Controller.Screen.print(" %d",i);
    //i=i+1;
    task::sleep(20);
  }
  if (returnValue == 1){
    Controller.Screen.print("true");
  }
  else {
    
  }
  return returnValue;
}

int hook_out_on_red_ring() {
  while (true)
  {
    ColorSensor.setLightPower(100, percent);
    color DetectedColor = ColorSensor.color();
    
    if (DetectedColor == color::red) {
      hook_out();
      break;
    }
    task::sleep(20);
  }
    return(0);
}


void arm_to_pickup(){
  // Set stopping to hold to keep arm stable in scoring position
  ArmMotor.setStopping(hold);
  isArmGoingToPickup = true;
  if (isArmGoingToPickup && RotationSensor.value() < 24200)
  {
    ArmMotor.spin(reverse);
  }
  
  // Going from resting postion to pickup position
  if (isArmGoingToPickup && RotationSensor.value() > 24400)
  {
    ArmMotor.spin(forward);
  }
}

void rotation_sensor_callback() {
  int rotationValue = RotationSensor.value();
  // Going from scoring postion to pickup position
  if (isArmGoingToPickup && rotationValue < 24200)
  {
    ArmMotor.spin(reverse);
  }
  // Going from resting postion to pickup position
  if (isArmGoingToPickup && rotationValue > 24400)
  {
    ArmMotor.spin(forward);
  }
  if (isArmGoingToPickup && 24200 < rotationValue && rotationValue < 24400)
  {
    ArmMotor.stop(hold);
    isArmGoingToPickup = false;
  }
}

void intake_ring_in() {
IntakeMotor.setVelocity(90, percent);
IntakeMotor.spin(reverse);
}

void intake_ring_in_skills() {
IntakeMotor.setVelocity(100, percent);
IntakeMotor.spin(reverse);
}

void intake_ring_out(){
  IntakeMotor.setVelocity(90, percent);
  IntakeMotor.spin(forward);
}

void intake_ring_out_skills(){
  IntakeMotor.setVelocity(100, percent);
  IntakeMotor.spin(forward);
}

void intake_stop() {
  IntakeMotor.setBrake(coast);
  IntakeMotor.stop();
}

void chain_stop() {
  ChainMotor.setBrake(coast);
  ChainMotor.stop();
}

void chain_ring_up() {
  ChainMotor.setVelocity(90,percent);
  ChainMotor.spin(forward);
}
void chain_ring_up_skills() {
  ChainMotor.setVelocity(100,percent);
  ChainMotor.spin(forward);
}
void chain_ring_down() {
  ChainMotor.setVelocity(90,percent);
  ChainMotor.spin(reverse);
}
void chain_ring_down_skills() {
  ChainMotor.setVelocity(100,percent);
  ChainMotor.spin(reverse);
}
void arm_up(){
  ArmMotor.spin(forward);
}

void arm_down(){
  ArmMotor.spin(reverse);
}

void arm_stop(){
  ArmMotor.stop(coast);
}

void robot_stop(){
  chassis.drive_stop(coast);
  chain_stop();
  intake_stop();

}


void hook_toggle() {
  // This callback has to be called with ButtonX
  if(Controller.ButtonX.pressing()) {
    if (isHookOut) {
      hook_in();
    } else {
      hook_out();
    }
  }
}

// sweeper_toggle has to be called with ButtonA
void sweeper_toggle() {
if(Controller.ButtonA.pressing())  {
  if (isSweeperDown) {
      sweeper_up();
   } else {
      sweeper_down();    }
  }
}

// hang_toggle has to be called with ButtonY
void hang_toggle() {
if(Controller.ButtonY.pressing())  {
  if (isHangUp) {
      HangPiston1.set(false);
      HangPiston2.set(false);
    } else {
      HangPiston1.set(true);
      HangPiston2.set(true);
  }
  // Toggle variable value
  isHangUp = !isHangUp; 
  }
}

void hook_in() {
  isHookOut = false;
  HookPiston.set(false);
}

void hook_out() {
  isHookOut = true;
  HookPiston.set(true);
}

void bouncing_intake_up() {
  BouncingIntakePiston.set(true);
}

void bouncing_intake_down() {
  BouncingIntakePiston.set(false);
}

void sweeper_down() {
  SweeperPiston.set(true);
  isSweeperDown = true;
}

void sweeper_up() {
  SweeperPiston.set(false);
  isSweeperDown = false;
}

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
  
 isHookOut = false;
 isSweeperDown = false;
 isManualSkillCorner1Running = false;

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

void goal_rush_red(){
//Grab middle mogo
hook_out();
chassis.drive_distance(-38, chassis.get_absolute_heading(), 7, 12);
fast_turn(30);
chassis.drive_distance(-10.3, chassis.get_absolute_heading(), 4, 12);
hook_in();
wait(0.2,seconds);
// Shove preload on the stake
chain_ring_up();
intake_ring_in();
//pull mogo away from middle
drive_short_distance(7.8);
//pick up bottom ring from stack
fast_turn(305);
bouncing_intake_up();
intake_ring_out();
drive_short_distance(9);
chassis.drive_distance(8, chassis.get_absolute_heading(), 4, 12);
chain_stop();
wait(0.1,seconds);
bouncing_intake_down();
intake_ring_in();
wait(0.3,seconds);
drive_short_distance(3);
//Drive back to let go of mogo
drive_short_distance(-13);
intake_stop();
hook_out();
wait(0.1,seconds);
drive_short_distance(12.5);
//pick up mogo 2
fast_turn(270);
drive_short_distance(-18);
chassis.drive_distance(-8, chassis.get_absolute_heading(), 3, 12);
drive_short_distance(-4);
hook_in();
intake_ring_out();
wait(0.3,seconds);
intake_ring_in();
chain_ring_up();
drive_short_distance(2);
wait(1.7,seconds);
fast_turn(315);
intake_stop();
//insert auton end below this comment
goal_rush_positive_corner_end_red();
}

void turnRight(){
  LeftBack.spin(reverse,3,volt);
  LeftMiddle.spin(reverse,3,volt);
  LeftFront.spin(reverse,3,volt);
  RightBack.spin(forward,3,volt);
  RightMiddle.spin(forward,3,volt);
  RightFront.spin(forward,3,volt);
}

void AITest(){
  vex::task detectBlueRings = task(look_for_blue_rings);
  while (true) {
    if(blueRingDetected == false){
      noBlueRingTimes += 1;
      turnRight();
      ringCertaintyLevel = 0;
    }
    if(blueRingDetected == true){
      blueRingTimes += 1;
      ringCertaintyLevel += 1;
      if (ringCertaintyLevel > 50){
        chassis.drive_distance(5);
        ringCertaintyLevel = 0;
      }
      
      //chassis.drive_with_voltage(3,3);
    }
    if(blueRingTimes > 1000){
      chassis.drive_distance(-40);
      blueRingTimes = 0;
    }
    if(noBlueRingTimes > 1000){
      chassis.drive_distance(-40);
      noBlueRingTimes = 0;
    }
    wait(5, msec);
  }
}

void goal_rush_positive_corner_end_red(){
  chassis.drive_distance(20, chassis.get_absolute_heading(), 12, 12);
}

void goal_rush_positive_corner_end_blue(){
  chassis.drive_distance(32.5, chassis.get_absolute_heading(), 12, 12);
}

void goal_rush_ladder_end_red(){
  chassis.drive_distance(-15, chassis.get_absolute_heading(), 12, 12);
}

void goal_rush_ladder_end_blue(){
  chassis.drive_distance(-10.5, chassis.get_absolute_heading(), 12, 12);
}

void goal_rush_blue(){
//Grab middle mogo
hook_out();
chassis.drive_distance(-35.5, chassis.get_absolute_heading(), 6.5, 12);
fast_turn(330);
drive_short_distance(-4.5);
chassis.drive_distance(-10.3, chassis.get_absolute_heading(), 3, 12);
//wait(0.05,seconds);
hook_in();
wait(0.2,seconds);
// Shove preload on the stake
chain_ring_up();
intake_ring_in();
//pull mogo away from middle
drive_short_distance(10.3);
//pick up bottom ring from stack
fast_turn(55);
//drop off middle goal
drive_short_distance(-5);
intake_stop();
hook_out();
wait(0.1,seconds);
drive_short_distance(5);
bouncing_intake_up();
intake_ring_out();
drive_short_distance(9.5);
chassis.drive_distance(8, chassis.get_absolute_heading(), 4, 12);
chain_stop();
wait(0.1,seconds);
bouncing_intake_down();
intake_ring_in();
wait(0.25,seconds);
drive_short_distance(1.5);
//Drive back to grab mogo
drive_short_distance(-5.5);
//pick up mogo 2
fast_turn(90);
drive_short_distance(-15.3);
chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
//drive_short_distance(-4);
hook_in();
intake_ring_out();
wait(0.25,seconds);
drive_short_distance(1);
intake_ring_in();
chain_ring_up();
wait(1.7,seconds);
fast_turn(45);
intake_stop();
//insert auton end below this comment
goal_rush_positive_corner_end_blue();
}

void one_stake_red(){
//four_point_auton_left without the touch
isHookOut = false;
vex::task hookOutOnWrongRingTask = task(hook_out_on_yellow);
preload_mogo_left_side();
wait(0.2,seconds);
drive_short_distance(-2);
fast_turn(90);
bouncing_intake_up();
intake_ring_out();
drive_short_distance(23);
bouncing_intake_down();
intake_ring_in();
chain_ring_up();
drive_short_distance(3);
wait(0.3,seconds);
intake_stop();
drive_short_distance(6);
fast_turn(190);
intake_ring_in();
drive_short_distance(7);
bouncing_intake_up();
intake_ring_out();
chassis.drive_distance(8, chassis.get_absolute_heading(), 5, 12);
bouncing_intake_down();
intake_ring_out();
wait(0.15,seconds);
intake_ring_in_skills();
chain_ring_up_skills();
wait(0.15,seconds);
drive_short_distance(-10);
fast_turn(210);
drive_short_distance(7);
bouncing_intake_up();
intake_ring_out();
chassis.drive_distance(8, chassis.get_absolute_heading(), 5, 12);
bouncing_intake_down();
intake_ring_in_skills();
chain_ring_up_skills();
wait(0.3,seconds);
chassis.drive_distance(-16, chassis.get_absolute_heading(), 12, 12);
fast_turn(80);
wait(0.2,sec); //so 4th ring scores for sure
hookOutOnWrongRingTask.stop();
//hook_out();
chassis.drive_distance(-31, chassis.get_absolute_heading(), 12, 12);
fast_turn(40);

}

void one_stake_blue(){
vex::task hookOutOnWrongRingTask = task(hook_out_on_red_ring);
//four_point_auton_left without the touch
preload_mogo_right_side();
wait(0.2,seconds);
drive_short_distance(-2);
fast_turn(270);
bouncing_intake_up();
intake_ring_out();
drive_short_distance(23);
bouncing_intake_down();
intake_ring_in();
chain_ring_up();
drive_short_distance(3);
wait(0.3,seconds);
intake_stop();
drive_short_distance(6);
fast_turn(170);
intake_ring_in();
drive_short_distance(7);
bouncing_intake_up();
intake_ring_out();
chassis.drive_distance(8, chassis.get_absolute_heading(), 5, 12);
bouncing_intake_down();
intake_ring_out();
wait(0.15,seconds);
intake_ring_in_skills();
chain_ring_up_skills();
wait(0.15,seconds);
drive_short_distance(-10);
fast_turn(145);
drive_short_distance(7);
bouncing_intake_up();
intake_ring_out();
chassis.drive_distance(8, chassis.get_absolute_heading(), 5, 12);
bouncing_intake_down();
intake_ring_in_skills();
chain_ring_up_skills();
wait(0.3,seconds);
chassis.drive_distance(-16, chassis.get_absolute_heading(), 12, 12);
fast_turn(280);
wait(0.25,sec);
hookOutOnWrongRingTask.stop();
//hook_out();
chassis.drive_distance(-32.2, chassis.get_absolute_heading(), 12, 12);
chassis.drive_stop(coast);
}

void preload_mogo_left_side () {
  hook_out();
  drive_short_distance(-8);
  fast_turn(330);
  drive_short_distance(-12);
  chassis.drive_distance(-9, chassis.get_absolute_heading(), 4, 12);
  hook_in(); //hook in
  chain_ring_up();
  intake_ring_in();
  //wait(0.1,seconds);
// Shove ring on the stake
  
  // wait(2,seconds);
  // chain_stop();
}
void preload_mogo_right_side () {
  hook_out();
  drive_short_distance(-8);
  fast_turn(30);
  drive_short_distance(-11);
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
  hook_in(); //hook in
  chain_ring_up();
  intake_ring_in();
  //wait(0.2,seconds);
  
  //wait(2,seconds);
  //chain_stop();
}


void four_point_auton_left(){ //Slot 8
  preload_mogo_left_side();
  drive_short_distance(-2);
  fast_turn(90);
  bouncing_intake_up();
  intake_ring_out();
  drive_short_distance(23);
  bouncing_intake_down();
  intake_ring_in();
  chain_ring_up();
  drive_short_distance(3);
  wait(1.5,seconds);
  intake_stop();
  drive_long_distance(-20);

  //take stake out
  //Touch central structure
  chassis.turn_to_angle(45);
  chassis.drive_distance(-11);
  hook_out(); //hook out
  wait(0.5,seconds);
  //hook_out()
  chassis.drive_stop(coast);
  
}

void pickup_bottom_ring_stack() {
  bouncing_intake_up();
  intake_ring_out();
  chassis.drive_distance(24);
  chain_stop();
  chassis.drive_distance(-5);
  bouncing_intake_down();
  intake_ring_in();
  chassis.drive_distance(9);
  wait(0.7,seconds);
  intake_stop();
}

// shove 2 rings on mogo
void four_point_auton_right(){ //Slot 7

  preload_mogo_right_side();
  drive_short_distance(-2);
  fast_turn(270);
  bouncing_intake_up();
  intake_ring_out();
  drive_short_distance(23);
  bouncing_intake_down();
  intake_ring_in();
  chain_ring_up();
  drive_short_distance(3);
  wait(1.5,seconds);
  intake_stop();
  drive_long_distance(-20);

  //take stake out
  //Touch central structure
  chassis.turn_to_angle(315);
  chassis.drive_distance(-11);
  hook_out(); //hook out
  wait(0.5,seconds);
  //hook_out()
  chassis.drive_stop(coast);
  
}
void score_ring_mogo(){
  hook_in(); //hook in
  wait(0.3,seconds);
// Shove ring on the stake
  intake_ring_out();
  wait(0.4,seconds);
  intake_ring_in();
  chain_ring_up();
  chassis.drive_distance(7);
  wait(0.8,seconds);
  chain_stop();
}

void six_point_two_mogo_right() { // slot 3
  Controller.Screen.print("sixpoint");
  preload_mogo_right_side(); //first ring done
  chassis.drive_distance(-2);
  chassis.turn_to_angle(270);
  //hook_out();
  pickup_bottom_ring_stack(); //get 2nd ring
  chassis.drive_distance(-20);
  hook_out();
  chassis.drive_distance(4);
  chassis.turn_to_angle(30);
  wait(0.1, seconds);
  chassis.drive_distance(-10.7);  
  chassis.drive_distance(-7, chassis.get_absolute_heading(), 3, 12);
  score_ring_mogo(); //score 2nd ring on 2nd mogo
  hook_out(); 
}

void six_point_two_mogo_left() { // slot 6
  Controller.Screen.print("sixpoint");
  preload_mogo_left_side(); //first ring done
  chassis.drive_distance(-2);
  chassis.turn_to_angle(90);
  chassis.drive_distance(1);
  //hook_out();
  pickup_bottom_ring_stack(); //get 2nd ring
  chassis.drive_distance(-21);
  hook_out();
  chassis.drive_distance(4);
  chassis.turn_to_angle(330);
  wait(0.1, seconds);
  chassis.drive_distance(-11.5);  
  chassis.drive_distance(-7, chassis.get_absolute_heading(), 3, 12);
  score_ring_mogo(); //score 2nd ring on 2nd mogo
  hook_out(); 
}

void three_point_auton_left(){ 
  hook_out();
  chassis.drive_distance(-8);
  chassis.turn_to_angle(330);
  wait(0.1,seconds);
  chassis.drive_distance(-16);
  chassis.drive_distance(-5);
  hook_in(); //hook in
  wait(0.5,seconds);
// Shove ring on the stake
  chain_ring_up();
  wait(2,seconds);
  chain_stop();
  hook_out();

  //take stake out
  //Touch central structure
  chassis.turn_to_angle(45);
  chassis.drive_distance(-15);
  hook_out(); //hook out
  wait(0.5,seconds);
  //hook_out()
  chassis.drive_stop(coast);
  
}

/**
 * The expected behavior is to return to the start angle, after making a complete turn.
 * This will be used for blue only.
 */
void wall_stake_auton_left(){  //slot 8
  ArmMotor.setVelocity(100,percent);
  arm_up();
  wait(1.3,seconds);
  //wait to score preolad on wall stakechassis.drive_distance(-26,chassis.get_absolute_heading(),12,12,1.5, skills_drive_settle_time, 1200);
  ArmMotor.setVelocity(50,percent);
  arm_down();
  drive_short_distance(-10);
  fast_turn(325);
  //turn to pick up top ring in front of wall stake
  bouncing_intake_up();
  //wait(0.2,seconds);
  drive_short_distance(16); 
  bouncing_intake_down();
  fast_turn(57);
  intake_stop();
  hook_out();
  drive_short_distance(-16);
  chassis.drive_distance(-8, chassis.get_absolute_heading(), 4, 12);
  drive_short_distance(-8);
  intake_ring_out();
  wait(0.1,seconds);
  hook_in();
  wait(0.2,seconds);
  arm_stop();
  //grab the mogo
  intake_ring_in();
  chain_ring_up();
  fast_turn(172);
  bouncing_intake_up();
  intake_ring_out();
  drive_short_distance(23);
  bouncing_intake_down();
  intake_ring_in_skills();
  chain_ring_up_skills();
  //grab next ring and score
  drive_short_distance(4);
  //Insert deisred auton end after this c
  left_pos_corner_wall_stake_auton_end();

}
void red_left_7pt_wall_stake_auton_end(){
  isHookOut = false;
  vex::task hookOutOnWrongRingTask = task(hook_out_on_yellow);
  fast_turn(263);
  drive_short_distance(7.2);
  bouncing_intake_up();
  intake_ring_out();
  chassis.drive_distance(8, chassis.get_absolute_heading(), 7, 12);
  bouncing_intake_down();
  intake_ring_in_skills();
  chain_ring_up_skills();
  wait(0.2,seconds);
  chassis.drive_distance(-30, chassis.get_absolute_heading(), 12, 12);
  fast_turn(136);
  hookOutOnWrongRingTask.stop();
  chassis.drive_distance(-30, chassis.get_absolute_heading(), 12, 12);
  chain_stop();
  chassis.drive_distance(-12, chassis.get_absolute_heading(), 12, 12);
  chassis.drive_stop(coast);
}

void left_pos_corner_wall_stake_auton_end(){
  wait(1.8,seconds);
  drive_short_distance(-18);
  fast_turn(132);
  intake_stop();
  drive_short_distance(32);
}

void left_default_ladder_wall_stake_auton_end(){
  wait(1.8,seconds);
  drive_short_distance(-18);
  fast_turn(132);
  chain_stop();
  drive_short_distance(-15.5);
}

void right_default_ladder_wall_stake_auton_end(){
  drive_short_distance(-20);
  wait(1.5,seconds);
  fast_turn(-135);
  chain_stop();
  drive_short_distance(-15);
}

void right_pos_corner_wall_stake_auton_end(){
  drive_short_distance(-20);
  wait(1.5,seconds);
  fast_turn(-130);
  drive_short_distance(20);
}

void blue_right_7pt_wall_stake_auton_end(){
  isHookOut = false;
  vex::task hookOutOnWrongRingTask = task(hook_out_on_red_ring);
  fast_turn(99);
  drive_short_distance(6.5);
  bouncing_intake_up();
  intake_ring_out();
  chassis.drive_distance(8, chassis.get_absolute_heading(), 7, 12);
  bouncing_intake_down();
  intake_ring_in_skills();
  chain_ring_up_skills();
  wait(0.35,seconds);
  chassis.drive_distance(-30, chassis.get_absolute_heading(), 12, 12);
  fast_turn(226);
  hookOutOnWrongRingTask.stop();
  wait(2,seconds);
  //chassis.drive_distance(-30, chassis.get_absolute_heading(), 12, 12);
  chain_stop();
  chassis.drive_distance(-11, chassis.get_absolute_heading(), 12, 12); 
}

void wall_stake_auton_right(){ //slot 7
  ArmMotor.setVelocity(100,percent);
  arm_up();
  wait(1.4,seconds);
  ArmMotor.setVelocity(50,percent);
  //wait to score preolad on wall stake
  arm_down();
  drive_short_distance(-10);
  fast_turn(40);
  //turn to pick up top ring in front of wall stake
  intake_ring_out();
  bouncing_intake_up();
  //intake_ring_in();
  drive_short_distance(16); 
  //ring is picked up
  bouncing_intake_down();
  fast_turn(306);
  intake_stop();
  hook_out();
  drive_short_distance(-16);
  chassis.drive_distance(-8, chassis.get_absolute_heading(), 4, 12);
  drive_short_distance(-8);
  intake_ring_out();
  wait(0.1,seconds);
  hook_in();
  wait(0.2,seconds);
  arm_stop();
  //grab the mogo
  intake_ring_in();
  chain_ring_up();
  fast_turn(185);
  bouncing_intake_up();
  intake_ring_out();
  drive_short_distance(23);
  bouncing_intake_down();
  intake_ring_in_skills();
  chain_ring_up_skills();
  //grab next ring and score
  drive_short_distance(4);
  //Insert desired end below this comment
  right_pos_corner_wall_stake_auton_end();

}


void manual_skills_corner_1_task(){
  bouncing_intake_down();
  chain_ring_up_skills();
  intake_ring_in_skills();
  wait(0.8,seconds);
  chain_stop();
  intake_stop();
  //Preload scored on alliance stake
  drive_short_distance(12.4);
  fast_turn(90);
  hook_out();  
  drive_short_distance(-10);
  chain_ring_down_skills(); //To keep the mogo in correct position
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12); 
  wait(0.2,seconds); //this can be reduced 0.1 sec
  hook_in();
  wait(0.3,seconds); //this can be reduced 0.1 sec;
}

//First part of skills, ends at the start of the long distance between mogos 1 and 2
 void skills_corner_1() {
  bouncing_intake_down();
  chain_ring_up_skills();
  intake_ring_in_skills();
  wait(0.8,seconds);
  chain_stop();
  intake_stop();
  //Preload scored on alliance stake
  drive_short_distance(12.4);
  fast_turn(90);
  hook_out();  
  drive_short_distance(-10);
  chain_ring_down_skills(); //To keep the mogo in correct position
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12); 
  drive_short_distance(-3);
  wait(0.1,seconds); //this can be reduced 0.1 sec
  hook_in();
  wait(0.2,seconds); //this can be reduced 0.1 sec
  //Grabbed onto 1st mogo
  fast_turn(0);
  chain_ring_up_skills();
  intake_ring_in_skills();
  drive_short_distance(22);
  //Picked up 1st ring 
  drive_short_distance(2.5);
  wait(0.1,seconds);
  fast_turn(270);
  drive_short_distance(29.5);
  //wait(0.2,seconds);
  //wait(0.5, seconds);
  //Picked 2st red ring, now scoring onto mogo
  drive_short_distance(-5);
  fast_turn(180);
  drive_short_distance(28);
  wait(0.2,seconds); //this can be reduced 0.1 sec
  drive_short_distance(6); // used to be 7
  wait(0.2, seconds); //this can be reduced 0.1 sec
  //Picked up 3rd and 4th ring, waiting to score them
  drive_short_distance(-13); // used to be _9.5,
  //skipped picked up 5th ring, not waiting to score it
  chassis.turn_to_angle(270,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(12);
  wait(0.3,seconds);
  drive_short_distance(-10);
  chassis.turn_to_angle(270,12,5,100, 1000);
  fast_turn(45);//used to be 42
  chassis.drive_distance(-18,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1000);
  chain_stop();
  intake_stop();
  chain_ring_down();
  wait(0.2,seconds); //this can be reduced 0.1 sec
  chain_stop();
  ChainMotor.stop();
  hook_out();
  wait(0.2,seconds); //this can be reduced 0.1 sec
  //realeased  1st stake into corner with 5 rings
  drive_short_distance(2);
  fast_turn(45);
  drive_short_distance(9.5); //13.6 inch at dublin
  fast_turn(-90);
  chassis.turn_to_angle(-90,12,1,220, skills_turn_timeout);
}

//Second part of skills, starts at the begining of distance between mogos 1 and 2, ends at the start of long distance between mogos 2 and 3
void skills_corner_2() {
  chassis.set_heading(-90);
  hook_out();
  intake_ring_in_skills();
  //chain_ring_up();
  chassis.drive_distance(-56, chassis.get_absolute_heading(), 6, 12);
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
  chassis.drive_distance(-8, chassis.get_absolute_heading(), 4, 12);
  intake_ring_out();
  wait(0.1,seconds); //Can his be reduced?
  hook_in();
  wait(0.2,seconds); //Can this be reduced?
  intake_stop();
  drive_short_distance(2);
  //Grabbed onto 2nd mogo


  //Reverse code of the 1st corner:
  //drive_short_distance(-2); //Previously was -4
  chassis.turn_to_angle(0,12,1,skills_turn_settle_time, skills_turn_timeout);
  chain_ring_up_skills();
  intake_ring_in_skills();
  drive_short_distance(22);
  //Picked up 1st ring 
  drive_short_distance(3);
  //wait(0.1,seconds); //this can be reduced 0.1 sec
  chassis.turn_to_angle(90,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(29);
  //wait(0.1,seconds); //this can be reduced 0.1 sec
  //wait(0.5, seconds);
  //Picked 2st red ring, now scoring onto mogo
  drive_short_distance(-4.5); //changed so robot doesn't bump into ring 5
  chassis.turn_to_angle(180,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(29);
  wait(0.2,seconds);
  //wait(0.5, seconds);
  drive_short_distance(6.5);
  wait(0.2, seconds); //this can be reduced 0.1 sec
  //Picked up 3rd and 4th ring, waiting to score them
  drive_short_distance(-12.5);
  // chassis.turn_to_angle(90,12,5,50, skills_turn_timeout);
  // drive_short_distance(12);
  // chain_stop();
  // wait(0.2,seconds);
  //Picked up 5th ring, waiting to score it
  chassis.turn_to_angle(90,12,5,100, 1000);
  chassis.turn_to_angle(315,12,1,skills_turn_settle_time, skills_turn_timeout);
  //wait(0.05,seconds);
  hook_out();
  wait(0.05,seconds);
  chassis.drive_distance(7);
  hook_in();
  wait(0.05,seconds);
  chassis.drive_distance(-27,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1000);
  chain_stop();
  intake_stop();
  chain_ring_down();
  wait(0.1,seconds); 
  chain_stop();
  ChainMotor.stop();
  // hook_out();
  //wait(0.2,seconds); //this can be reduced 0.1 sec

  //Go to pick up mogo with blue ring 
  drive_short_distance(2);
  fast_turn(315);
  drive_short_distance(15); //might have to change to 15
  chassis.turn_to_angle(0,12,1,220, skills_turn_timeout);
}

void skills_corner_3() {
  hook_out();
  intake_ring_in();
  chassis.drive_distance(60, chassis.get_absolute_heading(), 7, 12);//Change 8 back to 7 for more precision
  //make velocity here higher to cut 0.2 ish sec
  chassis.drive_distance(10, chassis.get_absolute_heading(), 7, 12);
  //1 ring in intake for wall stake
  intake_stop();
  chassis.turn_to_angle(146,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(-26);
  intake_stop();
  chassis.drive_distance(-11, chassis.get_absolute_heading(), 4, 12);
  wait(0.15,seconds); //Can this be reduced?
  hook_in();
  wait(0.3,seconds); //Can this be reduced?
  drive_short_distance(4);
  //grabbed onto mago, now going to push into corner
  chassis.turn_to_angle(107,12,1,skills_turn_settle_time, skills_turn_timeout);
  intake_ring_in();
  drive_short_distance(11);
  wait(0.15,seconds);
  intake_stop();
  chain_stop();
  chassis.drive_distance(14, chassis.get_absolute_heading(), 9, 12);
  drive_short_distance(-3);
  intake_ring_in();
  chassis.turn_to_angle(225,12,1,skills_turn_settle_time, skills_turn_timeout);
  hook_out();
  drive_short_distance(7);
  intake_stop();
  hook_in();
  chassis.drive_distance(-17.5,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1100);
  //wait(0.2,seconds);

  //Grab 4th mogo
  drive_short_distance(10.5);
  chassis.turn_to_angle(90,12,1,skills_turn_settle_time, skills_turn_timeout);
}

void skills_corner_4(){
  hook_out();
  chassis.set_heading(90);
  chassis.drive_distance(-36, chassis.get_absolute_heading(), 10, 12);
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
  intake_ring_out();
  chassis.drive_distance(-8, chassis.get_absolute_heading(), 11, 12);
  wait(0.1,seconds); 
  hook_in();
  intake_ring_in_skills();
  chain_ring_up_skills();
  wait(0.3,seconds);
  drive_short_distance(4);
  wait(0.5,sec);
  chassis.drive_distance(-26, chassis.get_absolute_heading(), 11, 12);
  //chain_stop();
  chassis.drive_distance(-20,chassis.get_absolute_heading(),6,12,1.5, skills_drive_settle_time, 1200);
  chain_stop();
  chain_ring_down();
  chassis.turn_to_angle(130,12,1,skills_turn_settle_time, skills_turn_timeout);
  hook_out();
  wait(0.05,seconds);
  chain_stop();
  chassis.drive_distance(6, chassis.get_absolute_heading(), 12, 12);
  hook_in();
  chain_stop();
  chassis.drive_distance(-26,chassis.get_absolute_heading(),12,12,1.5, skills_drive_settle_time, 1200);
}

void skills(){ //Slot 2
  Controller.Screen.print("skills");
  skills_corner_1();
  skills_corner_2();
  skills_corner_3();
  skills_corner_4();
}

void safe_skills(){ //Slot 2
  Controller.Screen.print("safe skills");
  skills_corner_1_safe();
  skills_corner_2_safe();
  skills_corner_3_safe();
  skills_corner_4_safe();
}

/**
 * Doesn't drive the robot, but just prints coordinates to the Brain screen 
 * so you can check if they are accurate to life. Push the robot around and
 * see if the coordinates increase like you'd expect.
 */

void three_point_auton_right(){ 
  hook_out();
  chassis.drive_distance(-8);
  chassis.turn_to_angle(30);
  wait(0.1,seconds);
  chassis.drive_distance(-16);
  chassis.drive_distance(-5);
  hook_in(); //hook in
  wait(0.5,seconds);
// Shove ring on the stake
  chain_ring_up();
  wait(2,seconds);
  chain_stop();
  hook_out();

  //take stake out
  //Touch central structure
  chassis.turn_to_angle(315);
  chassis.drive_distance(-15);
  hook_out(); //hook out
  wait(0.5,seconds);
  //hook_out()
  chassis.drive_stop(coast);
  
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

void skills_corner_1_safe() {
  bouncing_intake_down();
  chain_ring_up_skills();
  intake_ring_in_skills();
  wait(0.8,seconds);
  chain_stop();
  intake_stop();
  //Preload scored on alliance stake
  drive_short_distance(12.4);
  fast_turn(90);
  hook_out();  
  drive_short_distance(-10);
  chain_ring_down_skills(); //To keep the mogo in correct position
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12); 
  drive_short_distance(-3);
  wait(0.2,seconds); //this can be reduced 0.1 sec
  hook_in();
  wait(0.3,seconds); //this can be reduced 0.1 sec
  //Grabbed onto 1st mogo
  fast_turn(0);
  chain_ring_up_skills();
  intake_ring_in_skills();
  drive_short_distance(22);
  //Picked up 1st ring 
  drive_short_distance(2.5);
  wait(0.1,seconds);
  fast_turn(270);
  drive_short_distance(29.5);
  //wait(0.2,seconds);
  //wait(0.5, seconds);
  //Picked 2st red ring, now scoring onto mogo
  drive_short_distance(-5);
  fast_turn(180);
  drive_short_distance(28);
  wait(0.2,seconds); //this can be reduced 0.1 sec
  drive_short_distance(6); // used to be 7
  wait(0.2, seconds); //this can be reduced 0.1 sec
  //Picked up 3rd and 4th ring, waiting to score them
  drive_short_distance(-13); // used to be _9.5,
  //skipped picked up 5th ring, not waiting to score it
  // chassis.turn_to_angle(270,12,1,skills_turn_settle_time, skills_turn_timeout);
  // drive_short_distance(12);
  // wait(0.3,seconds);
  // drive_short_distance(-10);
  chassis.turn_to_angle(270,12,5,100, 1000);
  fast_turn(45);//used to be 42
  chassis.drive_distance(-19,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1200);
  chain_stop();
  intake_stop();
  chain_ring_down();
  wait(0.2,seconds); //this can be reduced 0.1 sec
  chain_stop();
  ChainMotor.stop();
  hook_out();
  wait(0.2,seconds); //this can be reduced 0.1 sec
  //realeased  1st stake into corner with 5 rings
  drive_short_distance(2);
  fast_turn(45);
  drive_short_distance(12); //13.6 inch at dublin
  fast_turn(-90);
  chassis.turn_to_angle(-90,12,1,220, skills_turn_timeout);

}

void skills_corner_2_safe() {
  chassis.set_heading(-90);
  hook_out();
  intake_ring_in_skills();
  //chain_ring_up();
  chassis.drive_distance(-55, chassis.get_absolute_heading(), 6, 12);
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
  chassis.drive_distance(-8, chassis.get_absolute_heading(), 4, 12);
  wait(0.2,seconds); //Can his be reduced?
  hook_in();
  wait(0.3,seconds); //Can this be reduced?
  drive_short_distance(2);
  //Grabbed onto 2nd mogo


  //Reverse code of the 1st corner:
  //drive_short_distance(-2); //Previously was -4
  chassis.turn_to_angle(0,12,1,skills_turn_settle_time, skills_turn_timeout);
  chain_ring_up_skills();
  intake_ring_in_skills();
  drive_short_distance(22);
  //Picked up 1st ring 
  drive_short_distance(3);
  //wait(0.1,seconds); //this can be reduced 0.1 sec
  chassis.turn_to_angle(90,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(29);
  //wait(0.1,seconds); //this can be reduced 0.1 sec
  //wait(0.5, seconds);
  //Picked 2st red ring, now scoring onto mogo
  drive_short_distance(-4.5); //changed so robot doesn't bump into ring 5
  chassis.turn_to_angle(180,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(29);
  wait(0.2,seconds);
  //wait(0.5, seconds);
  drive_short_distance(6);
  wait(0.2, seconds); //this can be reduced 0.1 sec
  //Picked up 3rd and 4th ring, waiting to score them
  drive_short_distance(-12);
  // chassis.turn_to_angle(90,12,5,50, skills_turn_timeout);
  // drive_short_distance(12);
  // chain_stop();
  // wait(0.2,seconds);
  //Picked up 5th ring, waiting to score it
  chassis.turn_to_angle(90,12,5,100, 1000);
  chassis.turn_to_angle(315,12,1,skills_turn_settle_time, skills_turn_timeout);
  wait(0.1,seconds);
  hook_out();
  wait(0.2,seconds);
  chassis.drive_distance(10);
  hook_in();
  wait(0.2,seconds);
  chassis.drive_distance(-30,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1200);
  chain_stop();
  intake_stop();
  chain_ring_down();
  wait(0.1,seconds); 
  chain_stop();
  ChainMotor.stop();
  // hook_out();
  //wait(0.2,seconds); //this can be reduced 0.1 sec

  //Go to pick up mogo with blue ring 
  drive_short_distance(2);
  fast_turn(315);
  drive_short_distance(15); //might have to change to 15
  chassis.turn_to_angle(0,12,1,220, skills_turn_timeout);

}

void skills_corner_3_safe() {
  hook_out();
  intake_ring_in();
  chassis.drive_distance(60, chassis.get_absolute_heading(), 7, 12);//Change 8 back to 7 for more precision
  //make velocity here higher to cut 0.2 ish sec
  chassis.drive_distance(10, chassis.get_absolute_heading(), 7, 12);
  intake_stop();
  //1 ring in intake for wall stake
  chassis.turn_to_angle(146,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(-26);
  chassis.drive_distance(-11, chassis.get_absolute_heading(), 4, 12);
  wait(0.2,seconds); //Can this be reduced?
  hook_in();
  wait(0.3,seconds); //Can this be reduced?
  drive_short_distance(4);
  //grabbed onto mago, now going to push into corner
  chassis.turn_to_angle(107,12,1,skills_turn_settle_time, skills_turn_timeout);
  drive_short_distance(10);
  chassis.drive_distance(15, chassis.get_absolute_heading(), 8, 12);
  chassis.turn_to_angle(225,12,1,skills_turn_settle_time, skills_turn_timeout);
  hook_out();
  drive_short_distance(7);
  hook_in();
  drive_short_distance(-16.5);
  //wait(0.2,seconds);

  //Grab 4th mogo
  drive_short_distance(9.2);
  chassis.turn_to_angle(90,12,1,skills_turn_settle_time, skills_turn_timeout);
}


void skills_corner_4_safe(){
  hook_out();
  chassis.set_heading(90);
  chassis.drive_distance(-34, chassis.get_absolute_heading(), 8, 12);
  chassis.drive_distance(-10, chassis.get_absolute_heading(), 4, 12);
  intake_ring_out();
  chassis.drive_distance(-6, chassis.get_absolute_heading(), 5, 12);
  wait(0.1,seconds); 
  hook_in();
  intake_ring_in_skills();
  chain_ring_up_skills();
  wait(1,seconds);
  chassis.drive_distance(-25, chassis.get_absolute_heading(), 9, 12);
  chassis.drive_distance(-20,chassis.get_absolute_heading(),4,12,1.5, skills_drive_settle_time, 1200);
  chain_ring_down();
  chassis.turn_to_angle(130,12,1,skills_turn_settle_time, skills_turn_timeout);
  hook_out();
  wait(0.1,seconds);
  chain_stop();
  chassis.drive_distance(6, chassis.get_absolute_heading(), 12, 12);
  hook_in();
  chain_stop();
  chassis.drive_distance(-27,chassis.get_absolute_heading(),11,12,1.5, skills_drive_settle_time, 1200);
  //chassis.drive_distance(5, chassis.get_absolute_heading(), 4, 12);
  
}