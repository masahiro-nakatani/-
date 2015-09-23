//
// ファイル名 : Main.cpp
// 概要       : 2輪倒立振子ライントレースロボットのTOPPERS/ATK1(OSEK)用C++サンプルプログラム
//



extern "C"
{
#include "rtoscalls.h"
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

//=============================================================================
// TOPPERS/ATK1の宣言


// タスクの起動周期[msec]
#define DRIVE_PERIOD       (4)
#define BACKGROUND_PERIOD (48)


};
