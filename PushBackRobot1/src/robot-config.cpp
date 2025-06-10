#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen.
brain  Brain;

controller Controller = controller();

//The motor constructor takes motors as (port, ratio, reversed), so for example
motor LeftFront = motor(PORT9, ratio6_1, false);
motor LeftMiddle = motor(PORT13, ratio6_1, false);
motor LeftBack = motor(PORT11, ratio6_1, false);
motor RightFront = motor(PORT10, ratio6_1, true);
motor RightMiddle = motor(PORT14, ratio6_1, true);
motor RightBack = motor(PORT15, ratio6_1, true);
motor ChainMotor = motor(PORT16, ratio6_1, true);
rotation RotationSensor = rotation(PORT8);
optical ColorSensor = optical(PORT19);
aivision AISensor = aivision(PORT5);
motor IntakeMotor = motor(PORT17, ratio6_1, true);
motor ArmMotor = motor(PORT6, ratio6_1, false);
digital_out HookPiston = digital_out(Brain.ThreeWirePort.B);
digital_out HangPiston1 = digital_out(Brain.ThreeWirePort.D);
digital_out HangPiston2 = digital_out(Brain.ThreeWirePort.E);
digital_out BouncingIntakePiston = digital_out(Brain.ThreeWirePort.A);
digital_out SweeperPiston = digital_out(Brain.ThreeWirePort.C);

//Add your devices below, and don't forget to do the same in robot-config.h:

void vexcodeInit( void ) {
  RotationSensor.changed(rotation_sensor_callback);
  }