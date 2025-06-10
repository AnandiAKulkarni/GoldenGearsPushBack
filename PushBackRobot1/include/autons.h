#pragma once
#include "JAR-Template/drive.h"
#include "vex.h"

class Drive;

extern Drive chassis;
// extern bool isHookOut;
extern bool isHangDown;
extern bool isArmGoingToPickup;
extern const float skills_turn_settle_time;
extern const float skills_turn_timeout;

void default_constants();