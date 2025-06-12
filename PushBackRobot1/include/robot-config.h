using namespace vex;

extern brain Brain;
extern controller Controller;
//Add your devices below, and don't forget to do the same in robot-config.cpp:

extern motor LeftFront;
extern motor LeftBackBottom;
extern motor LeftBackTop;
extern motor RightFront;
extern motor RightBackBottom;
extern motor RightBackTop;
extern motor LeftIntakeMotor;
extern motor RightIntakeMotor;
extern motor BackRollers;
extern motor FrontRoller;
extern digital_out MiddlePiston;
extern digital_out HighPiston;
extern aivision AISensor;
// extern rotation RotationSensor;
// extern optical ColorSensor;
// extern aivision AISensor;
void  vexcodeInit( void );