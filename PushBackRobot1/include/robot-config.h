using namespace vex;

extern brain Brain;
extern controller Controller;
//Drivetrain motors
extern motor LeftFront;
extern motor LeftBackBottom;
extern motor LeftBackTop;
extern motor RightFront;
extern motor RightBackBottom;
extern motor RightBackTop;
//Intake motors
extern motor BoxIntake;
extern motor Chain;
extern motor HighRoller;
//Pistons
extern digital_out IntakePiston;
extern digital_out HighPiston;
//AI Vision
extern aivision AISensor;
void  vexcodeInit( void );