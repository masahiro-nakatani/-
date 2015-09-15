#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"

class Mission_MisteryCircle : public Mission {
private:
    enum LocalState{
        P0 = 0,
        P1,
        P2,
        P3,
        P4
    };

public:
	static const Vec patternRR_[]; // 右右
	static const S16 patternRR_num_;
	static const S32 patternRR_time_[];

	static const Vec patternRL_[]; // 右左
	static const S16 patternRL_num_;
	static const S32 patternRL_time_[];

	static const Vec patternLR_[]; // 左右
	static const S16 patternLR_num_;
	static const S32 patternLR_time_[];

	static const Vec patternLL_[]; // 左左
	static const S16 patternLL_num_;
	static const S32 patternLL_time_[];

    Mission_MisteryCircle(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		// A, C: RIGHT
		// B, D: LEFT
		// false, false: A-C
		// false, true : A-D
		// true,  false: B-C
		// true,  true : B-D
		Robot::Instance().GetLCD().cursor(0,2);
		if (CourseInfo::Aquarius1 == false && CourseInfo::Aquarius2 == false) {
			tec_.Init(ni, patternRR_, patternRR_time_, patternRR_num_);
			Robot::Instance().GetLCD().putf("s", "RR");
		} else if (CourseInfo::Aquarius1 == false && CourseInfo::Aquarius2 == true) {
			tec_.Init(ni, patternRL_, patternRL_time_, patternRL_num_);
			Robot::Instance().GetLCD().putf("s", "RL");
		} else if (CourseInfo::Aquarius1 == true && CourseInfo::Aquarius2 == false) {
			tec_.Init(ni, patternLR_, patternLR_time_, patternLR_num_);
			Robot::Instance().GetLCD().putf("s", "LR");
		} else if (CourseInfo::Aquarius1 == true && CourseInfo::Aquarius2 == true) {
			tec_.Init(ni, patternLL_, patternLL_time_, patternLL_num_);
			Robot::Instance().GetLCD().putf("s", "LL");
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
				cmd = tec_pid_.Calclate(ri.light_sensor_val, 50, Technic::LEFT);
				ret = true;
			}
			break;

		case P2:
			if (IsTimeup()) {
				ret = false; // 終了.
				Robot::Instance().Beep(Robot::A2);
			} else {
				cmd = tec_pid_.Calclate(ri.light_sensor_val, 100, Technic::LEFT);
				ret = true;
			}
			break;

		default:
			ret = false; // 終了.
			break;
		}

        return ret;
    }


    virtual char* Name(void){
        return "Mission_MisteryCircle";
    }

private:

    TecBezierTrace tec_;
	TecPIDTrace tec_pid_;

//    static Mission_MisteryCircle instance_;

    LocalState local_status_;

};


//Mission_MisteryCircle Mission_MisteryCircle::instance_;

const Vec Mission_MisteryCircle::patternRR_[] = { // 右右
	Vec(0, 0, true), Vec(480, 0, true), Vec(480, -348, true),
	Vec( 425, -620, true), Vec( -290,  -400, true),
	Vec(-290,  -520, true), Vec(-290,  -710, true),
	Vec(-240,  -813, true), Vec(-359,  -943, true),
	Vec(-608, -1236, true), Vec(-410, -1240, true),
	Vec(-257, -1411, true), Vec( -84, -1400, true),
};
const S16 Mission_MisteryCircle::patternRR_num_ = sizeof Mission_MisteryCircle::patternRR_ / sizeof Mission_MisteryCircle::patternRR_[0];
const S32 Mission_MisteryCircle::patternRR_time_[] = {
	2000, 2000, 1000, 500, 1000, 3000, // [msec]
};

const Vec Mission_MisteryCircle::patternRL_[] = { // 右左
	Vec(0, 0, true), Vec(530, 0, true), Vec(530, -348, true),
	Vec(475, -620, true), Vec(-290,  -400, true),
	Vec(-550, -330, true), Vec(-550,  -670, true),
	Vec(-530, -800, true), Vec(-510,  -900, true),
	Vec(-465, -1050, true), Vec(-290, -1200, true),
};
const S16 Mission_MisteryCircle::patternRL_num_ = sizeof Mission_MisteryCircle::patternRL_ / sizeof Mission_MisteryCircle::patternRL_[0];
const S32 Mission_MisteryCircle::patternRL_time_[] = {
	2000, 3000, 3000, 2000, 2000, // [msec]
};

const Vec Mission_MisteryCircle::patternLR_[] = { // 左右
	Vec(0, 0, true), Vec(105, 0, true), Vec(105, -320, true),
	Vec(105, -520, true), Vec(-90,   -420, true),
	Vec(-240,  -520, true), Vec(-240,  -710, true),
	Vec(-240,  -813, true), Vec(-359,  -943, true),
	Vec(-608, -1240, true), Vec(-410, -1240, true),
	Vec(-257, -1410, true), Vec( -84, -1410, true),
};
const S16 Mission_MisteryCircle::patternLR_num_ = sizeof Mission_MisteryCircle::patternLR_ / sizeof Mission_MisteryCircle::patternRR_[0];
const S32 Mission_MisteryCircle::patternLR_time_[] = {
	2000, 3000, 1000, 500, 2000, 3000,// [msec]
};

const Vec Mission_MisteryCircle::patternLL_[] = { // 左左
	Vec(0, 0, true), Vec(105, 0, true), Vec(105, -320, true),
	Vec(  105, -520, true), Vec(-90,   -420, true),
	Vec(-650, -430, true), Vec(-650,  -650, true),
	Vec(-632, -800, true), Vec(-563,  -840, true),
	Vec(-495, -1050, true), Vec(-340, -1200, true),
};
const S16 Mission_MisteryCircle::patternLL_num_ = sizeof Mission_MisteryCircle::patternLL_ / sizeof Mission_MisteryCircle::patternLL_[0];
const S32 Mission_MisteryCircle::patternLL_time_[] = {
	2000, 2000, 2000, 2000, 2000// [msec]
};
