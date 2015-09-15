#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"

class Mission_DriftTurn : public Mission {
private:
    enum LocalState{
        P0 = 0,
        P1,
        P2,
        P3,
        P4
    };

public:
	static const Vec patternLR_[]; // 左右
	static const S16 patternLR_num_;
	static const S32 patternLR_time_[];

	static const Vec patternRL_[]; // 右左
	static const S16 patternRL_num_;
	static const S32 patternRL_time_[];

    Mission_DriftTurn(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		CourseInfo::PetBottle1=true;		// Debug
		Robot::Instance().GetLCD().cursor(0,2);
		if (CourseInfo::PetBottle1 == true) {
			tec_.Init(ni, patternLR_, patternLR_time_, patternLR_num_);
			Robot::Instance().GetLCD().putf("s", "LR");
		} else if (CourseInfo::PetBottle1 == false) {
			tec_.Init(ni, patternRL_, patternRL_time_, patternRL_num_);
			Robot::Instance().GetLCD().putf("s", "RL");
		}
		Robot::Instance().GetLCD().disp();
		local_status_ = P0;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){
		bool ret = true;

		switch (local_status_) {
		case P0:
			cmd = tec_.Calclate(ni.pos);
			if (tec_.isLastCtlPoints() && evf.is_SetEvent(Robot::EV_LIGHT_BLACK)) {
				// 最後の制御点列で黒を検知したら終了。
				Robot::Instance().Beep();
				local_status_ = P1;
				ret = true;
				tec_pid_.Init();
				timer_start (2000);
			}
			break;

		case P1:
			if (IsTimeup()) {
				Robot::Instance().Beep(Robot::B);
				local_status_ = P2;
				ret = true;
				timer_start (7000);
			} else {
				cmd = tec_pid_.Calclate(ri.light_sensor_val, 50, Technic::RIGHT);
				ret = true;
			}
			break;

		case P2:
			if (IsTimeup()) {
				ret = false; // 終了.
				Robot::Instance().Beep(Robot::A2);
			} else {
				cmd = tec_pid_.Calclate(ri.light_sensor_val, 100, Technic::RIGHT);
				ret = true;
			}
			break;

		default:
			ret = false; // 終了.
			break;
		}

		cmd.param3 = 0;
        return ret;
    }


    virtual char* Name(void){
        return "Mission_DriftTurn";
    }

private:

    TecBezierTrace tec_;
	TecPIDTrace tec_pid_;

//    static Mission_DriftTurn instance_;

    LocalState local_status_;

};


//Mission_DriftTurn Mission_DriftTurn::instance_;

const Vec Mission_DriftTurn::patternLR_[] = { // 左右(ルートa)
	Vec(0, 0, true), Vec(237, 255, true), Vec(0, 435, true),
	Vec(-228,595,true),Vec(25,860,true),
};
const S16 Mission_DriftTurn::patternLR_num_ = sizeof Mission_DriftTurn::patternLR_ / sizeof Mission_DriftTurn::patternLR_[0];
const S32 Mission_DriftTurn::patternLR_time_[] = {
	2000, 2000, // [msec]
};

const Vec Mission_DriftTurn::patternRL_[] = { // 右左(ルートb)
	Vec(0, 0, true), Vec(-237, 255, true), Vec(0, 435, true),
	Vec(228,595,true),Vec(25,860,true),
};
const S16 Mission_DriftTurn::patternRL_num_ = sizeof Mission_DriftTurn::patternRL_ / sizeof Mission_DriftTurn::patternRL_[0];
const S32 Mission_DriftTurn::patternRL_time_[] = {
	2000, 2000, // [msec]
};
