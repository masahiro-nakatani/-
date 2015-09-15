// PIDController.h

#ifndef _PID_CONTROLLER_H_
#define _PID_CONTROLLER_H_

#include "ecrobot_types.h"

typedef struct {
	F32 kp;
	F32 ki;
	F32 kd;
} PIDParam;

extern PIDParam pid_light_param;
extern PIDParam pid_direction_param;

/**
 * PID制御
 * 目標値との偏差に対して回転力の増減値を計算します。
 * 光センサの値を制御対象とする。（他のセンサ値でもできるはず）
 * @param kp P制御係数
 * @param ki I制御係数
 * @param kd D制御係数
 * @param e0 現在値と目標値の偏差
 * @param e1 前回値と目標値の偏差
 * @param e2 前々回値と目標値の偏差
 * @return rot 回転力の増減値
 */
void pid_calc_rotation (F32 kp, F32 ki, F32 kd, F32 e0, F32 e1, F32 e2, F32* rot);

/**
 * PID制御
 * 制御係数をまとめたラッパー関数
 * @param p  PID制御係数
 * @param e0 現在値と目標値の偏差
 * @param e1 前回値と目標値の偏差
 * @param e2 前々回値と目標値の偏差
 * @return rot 回転力の増減値
 */
void pid_calc_rotation (PIDParam* p, F32 e0, F32 e1, F32 e2, F32* rot);

/**
 * PID制御
 * 制御係数を変更します。
 * @param kp P制御係数
 * @param ki I制御係数
 * @param kd D制御係数
 */
void pid_change_param(F32 kp, F32 ki, F32 kd);

#endif // _PID_CONTROLLER_H_
