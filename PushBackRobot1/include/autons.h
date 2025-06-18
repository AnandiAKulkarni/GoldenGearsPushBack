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
void intake_block_in();
void intake_stop();
int look_for_blue_blocks();
void AITest();
void default_constants();
void turnRight();
void scoreInMiddle();
void scoreInHigh();
void scoreInLow();