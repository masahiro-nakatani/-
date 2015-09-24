#ifndef _INCLUDE_NAVIGATOR_H_
#define _INCLUDE_NAVIGATOR_H_
#include "PositionEstimation.h"
#include "jiro_type.h"
#include "utils.h"

class Navigator {
private:
	static const F64 TIRE_RADIUS   = 41.34061266;//!< タイヤ半径
	//static const F64 TIRE_DISTANCE = 85;
	//static const F64 TIRE_RADIUS   = 45.34061266;//!< タイヤ半径
	static const F64 TIRE_DISTANCE = 160;		//!< タイヤ幅
	static const F64 MAX_TURN_DEGREE = 100;		//!< 最大回転指定値(-100 - 100)

public:
	static Navigator& Instance(void);
	/**
	* 走行体の傾きを計算
	 @param encoderL 左モータエンコーダ値
	 @param encoderR 右モータエンコーダ値
	 @return 傾き[-360,360]
	*/
	static F64 calculateDAngles( S32 encoderL, S32 encoderR )
	{
		return TIRE_RADIUS * ( encoderR - encoderL ) / TIRE_DISTANCE;
	}
	/**
	 走行体の傾きをモータ値に変換
	 @param encoderL 左モータエンコーダ値
	 @param encoderR 右モータエンコーダ値
	 @return 傾き[-100,100]
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

    // 保持情報
    NavInfo info_;
	RobotInfo r_info_;

	// 角度補正用
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
