#ifndef _INCLUDE_JIRO_TYPE_H_
#define _INCLUDE_JIRO_TYPE_H_

#include "PositionEstimation.h"
#include "vec.h"

//************************************
// ���{�b�g���
struct RobotInfo{
    S16 light_sensor_val;
    S32 rot_encoderL_val;
    S32 rot_encoderR_val;
    S32 rot_encoderT_val; // �����ۃG���R�[�_�l
    S16 gyro_sensor_val;
	U16 battery_val;
};

// �i�r�Q�[�^���
struct NavInfo {
	Position pos;	//�ʒu���
	short areaCode;	//�G���A���
};

class RobotCmd{
  public:
	
    enum DRIVE_MODE{
        NORMAL_MODE = 0,
        DIRECT_MODE
    };
	
	enum {
		NO_TAIL_CNTL = -255, // �ݒ�Ȃ��i�O��ݒ�l�ł����ۂ𐧌䂷��j
	};

  RobotCmd(void)
	  : Mode(NORMAL_MODE),
		param1(0),
		param2(0),
		param3(NO_TAIL_CNTL)
			{}
	
  RobotCmd(S16 p1, S16 p2, S16 p3=NO_TAIL_CNTL, DRIVE_MODE m=NORMAL_MODE)
	  : Mode(m),
		param1(p1),
		param2(p2),
		param3(p3)
		{}


    DRIVE_MODE Mode;

	// NORMAIL_MODE Speed,Turn
	// DIRECT_MODE Motor_L,MotorR
	// Speed -100�`100
	// Turn -100�`100
	// PWM -100�`100
    S16 param1;         // Speed [%] or MotorL [PWM] out
    S16 param2;         // Turn  [%] or MotorR [PWM] out
    S16 param3;         // Tail motor [degree] out

};

#endif
