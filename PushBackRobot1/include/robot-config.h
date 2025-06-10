using namespace vex;

extern brain Brain;
extern controller Controller;
//Add your devices below, and don't forget to do the same in robot-config.cpp:

extern motor LeftFront;
extern motor LeftMiddle;
extern motor LeftBack;
extern motor RightFront;
extern motor RightMiddle;
extern motor RightBack;
extern motor ChainMotor;
extern motor IntakeMotor;
extern motor ArmMotor;
extern digital_out HookPiston;
extern digital_out HangPiston1;
extern digital_out HangPiston2;
extern digital_out BouncingIntakePiston;
extern digital_out SweeperPiston;
extern rotation RotationSensor;
extern optical ColorSensor;
extern aivision AISensor;
void  vexcodeInit( void );