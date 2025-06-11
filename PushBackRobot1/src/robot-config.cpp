#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen.
brain  Brain;

controller Controller = controller();

//The motor constructor takes motors as (port, ratio, reversed), so for example
motor LeftFront = motor(PORT4, ratio6_1, false);
motor LeftBackTop = motor(PORT3, ratio6_1, true);
motor LeftBackBottom = motor(PORT5, ratio6_1, false);
motor RightFront = motor(PORT2, ratio6_1, true);
motor RightBackBottom = motor(PORT9, ratio6_1, true);
motor RightBackTop = motor(PORT10, ratio6_1, false);
motor LeftIntakeMotor = motor(PORT6, ratio6_1, true);
motor RightIntakeMotor = motor(PORT7, ratio6_1, false);
motor BackRollers = motor(PORT8, ratio6_1, true);
motor FrontRoller = motor(PORT17, ratio6_1, true);
//optical ColorSensor = optical(PORT19);
//aivision AISensor = aivision(PORT5);
digital_out MiddlePiston = digital_out(Brain.ThreeWirePort.H);
digital_out HighPiston = digital_out(Brain.ThreeWirePort.G);

//Add your devices below, and don't forget to do the same in robot-config.h:

void vexcodeInit( void ) {
  // RotationSensor.changed(rotation_sensor_callback);
  }