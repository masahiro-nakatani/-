#ifndef _INCLUDE_NAVIGATOR_H_
#define _INCLUDE_NAVIGATOR_H_
#include "PositionEstimation.h"
#include "jiro_type.h"
#include "utils.h"

class Navigator {
private:
	static const F64 TIRE_RADIUS   = 41.34061266;//!< �^�C�����a
	//static const F64 TIRE_DISTANCE = 85;
	//static const F64 TIRE_RADIUS   = 45.34061266;//!< �^�C�����a
	static const F64 TIRE_DISTANCE = 160;		//!< �^�C����
	static const F64 MAX_TURN_DEGREE = 100;		//!< �ő��]�w��l(-100 - 100)

public:
	static Navigator& Instance(void);
	/**
	* ���s�̂̌X�����v�Z
	 @param encoderL �����[�^�G���R�[�_�l
	 @param encoderR �E���[�^�G���R�[�_�l
	 @return �X��[-360,360]
	*/
	static F64 calculateDAngles( S32 encoderL, S32 encoderR )
	{
		return TIRE_RADIUS * ( encoderR - encoderL ) / TIRE_DISTANCE;
	}
	/**
	 ���s�̂̌X�������[�^�l�ɕϊ�
	 @param encoderL �����[�^�G���R�[�_�l
	 @param encoderR �E���[�^�G���R�[�_�l
	 @return �X��[-100,100]
	*/
	static S16 convertToMotorValue( S32 encoderL, S32 encoderR )
	{
		return static_cast<S16>(
			calculateDAngles( encoderL, encoderR ) / 360 * MAX_TURN_DEGREE
			);
	}
	NavInfo GetInfo(const RobotInfo&);
	void SetInfo (F32 x, F32 y, F32 ang);
	void Init();
	void ResetEncoder();
	Position GetNowPos ();
	void StartDirCorrection (const F32, const F32);

private:

	void CalcDirCorrection ();

    // �ێ����
    NavInfo info_;
	RobotInfo r_info_;

	// �p�x�␳�p
	Position correct_pos_org_;
	F32  correct_target_dist_;
	F32  correct_target_dir_;
	F32  correct_sum_dir_;
	S32  correct_num_;
	bool isCorrectingAng;

    Navigator();
    ~Navigator(void);

    static Navigator* theInstance_;
};


#endif
