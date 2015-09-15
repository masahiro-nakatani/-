#ifndef _INCLUDE_NAVIGATOR_H_
#define _INCLUDE_NAVIGATOR_H_
#include "PositionEstimation.h"
#include "InfoType.h"
#include "Utils.h"

class Navigator {
private:
	static const F64 TIRE_RADIUS   = 41.3802852;
	static const F64 TIRE_DISTANCE = 80;

public:
	static Navigator& Instance(void);
	NavInfo GetInfo(const RobotInfo&);
	void SetInfo (F64 x, F64 y, F64 ang);
	void Init();
	void ResetEncoder();
	Position Getnow_positionition ();
	void StartDirCorrection (const F32, const F32);

private:

	void CalcDirCorrection ();

    // ï€éùèÓïÒ
    NavInfo info_;
	RobotInfo r_info_;

	// äpìxï‚ê≥óp
	Position correct_pos_org_;
	F32  correct_target_dist_;
	F32  correct_target_dir_;
	F32  correct_sum_dir_;
	S32  correct_num_;
	bool is_correcting_angle;

    Navigator();
    ~Navigator(void);

    static Navigator* theInstance_;
};


#endif
