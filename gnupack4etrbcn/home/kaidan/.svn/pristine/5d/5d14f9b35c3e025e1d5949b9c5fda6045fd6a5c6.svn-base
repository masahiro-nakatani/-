#ifndef _INCLUDE_INFO_TYPE_H_
#define _INCLUDE_INFO_TYPE_H_

/**
 * @file InfoType.h
 * @brief 共通で使用する値クラスを定義する
 */

#include "PositionEstimation.h"
#include "vec.h"

/**
 * ロボットの各センサとバッテリの値を保持する
 */
struct RobotInfo{
    S16 light_sensor_val; //!< 輝度値センサの値
    S32 rot_encoderL_val; //!< 左モータのエンコーダ値
    S32 rot_encoderR_val; //!< 右モータのエンコーダ値
    S32 rot_encoderT_val; //!< しっぽエンコーダ値
    S16 gyro_sensor_val;  //!< ジャイロセンサの値
    U16 battery_val;      //!< バッテリ残量値
};

/**
 * ロボットの位置とエリア情報を保持する
 */
struct NavInfo {
	Position pos;   //!< 位置情報
	short areaCode;	//!< エリア情報
};

/**
 * ロボットの走行指示に関するパラメータを保持する。
 *
 * @note RobotCmd は3つのパラメータを持つが、モードに応じて第1,第2パラメータの解釈が異なる。
 * @todo サブクラス化してRobotCmd{Normal,Direct}を作るべき。
 */
class RobotCmd{
public:

    enum DRIVE_MODE{
        NORMAL_MODE = 0, //<! 速度とロボットの向きを保持するモード。
        DIRECT_MODE      //<! 左右モータの回転数を直接設定するモード。
    };

    enum {
        NO_TAIL_CNTL = -255, // 設定なし（前回設定値でしっぽを制御する）
    };

    /**
     * 各パラメータは0として RobotCmd::NORMAL_MODE で生成する。しっぽは RobotCmd::NO_TAIL_CNTLを設定。
     */
    RobotCmd(void)
        : Mode(NORMAL_MODE),
          param1(0),
          param2(0),
          param3(NO_TAIL_CNTL)
        {}

    /**
     * 任意の値でパラメータを設定する。
     *
     * @param p1 NORMAL_MODEなら速度(%)[-100, 100], DIRECT_MODEなら左モータのPWM値[-100, 100]
     * @param p2 NORMAL_MODEなら向き(%)[-100, 100], DIRECT_MODEなら右モータのPWM値[-100, 100]
     * @param p3 しっぽの回転角度(degree) 省略すると NO_TAIL_CNTL が設定される。
     * @param m  モード。 省略すると NORMAL_MODE が設定される。
     *
     * @todo p3の値域不明
     */
    RobotCmd(S16 p1, S16 p2, S16 p3=NO_TAIL_CNTL, DRIVE_MODE m=NORMAL_MODE)
        : Mode(m),
          param1(p1),
          param2(p2),
          param3(p3)
		{}


    DRIVE_MODE Mode;

	// NORMAIL_MODE Speed,Turn
	// DIRECT_MODE Motor_L,MotorR
	// Speed -100～100
	// Turn -100～100
	// PWM -100～100
    S16 param1; //!< NORMAL_MODEならSpeed(%)[-100, 100], DIRECT_MODEなら左モータのPWM値[-100, 100]
    S16 param2; //!< NORMAL_MODEなら向き(%)[-100, 100], DIRECT_MODEなら右モータの[PWM][-100, 100]
    S16 param3; //!< しっぽモータの回転角度(degree)
};

#endif
