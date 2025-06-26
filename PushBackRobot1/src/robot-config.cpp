#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen.
brain  Brain;
controller Controller = controller();

//The motor constructor takes motors as (port, ratio, reversed)
//Drivetrain motors
motor LeftFront = motor(PORT14, ratio6_1, true);
motor LeftBackTop = motor(PORT2, ratio6_1, false);
motor LeftBackBottom = motor(PORT6, ratio6_1, true);
motor RightFront = motor(PORT11, ratio6_1, false);
motor RightBackBottom = motor(PORT16, ratio6_1, false);
motor RightBackTop = motor(PORT15, ratio6_1, true);
//Intake motors
motor BoxIntake = motor(PORT10, ratio6_1, true);
motor Chain = motor(PORT8, ratio6_1, true);
motor HighRoller = motor(PORT9, ratio6_1, true);
//Pistons
digital_out IntakePiston = digital_out(Brain.ThreeWirePort.H);
digital_out HighPiston = digital_out(Brain.ThreeWirePort.G);
//AI Vision is defined in autons.cpp
//GPS
gps GPSSensor = gps(PORT3, 0.00, 0.00, inches, 180);

void vexcodeInit( void ) {
  //this is used to calibrate everything before the game starts
  }