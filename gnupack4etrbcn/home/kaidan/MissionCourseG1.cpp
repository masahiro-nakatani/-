#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class Mission_CourseG1 : public Mission {
private:
	enum LocalState {
		MISSION_START,
		DETECT_GRAY_ZONE,
		MISSION_NEXT,
	};


public:
    Mission_CourseG1(S32 timer, S16 speed, S16 tail, Technic::DIRECTION edge)
		: Mission()
	{
		timer_ = timer;
		speed_ = speed;
		tail_ = tail;
		edge_ = edge;
    }

    virtual void Init(RobotInfo ri, NavInfo ni)
	{
        pid_trace_.Init();
		if (0 < timer_) {
			TimerStart(timer_);
		}
		local_status_ = MISSION_START;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd)
	{
        bool ret = true;

		switch (local_status_) {
		case MISSION_START:		// 単純走行モード.
			// 走行計算.
			cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = tail_;
			// 終了条件：指定時間で終了.
			if (IsTimeup()) {
				Robot::Instance().Beep(Robot::C);
				local_status_ = DETECT_GRAY_ZONE;
			}
			break;

		case DETECT_GRAY_ZONE:	// グレーゾーン検知モード.
			// 走行計算.
			cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = tail_;
			// 終了条件：グレー検知で終了.
			if (evf.is_SetEvent(Robot::EVENT_MARKER)) {
				Robot::Instance().Beep(Robot::D);
				local_status_ = MISSION_NEXT;
			}
			break;

		case MISSION_NEXT:
			// G1前半ミッション終了.
			Robot::Instance().Beep(Robot::E);
			ret = false;
			TimerStart(1000);
			break;
		}

        return ret;
    }


private:
	S32 timer_;
	S16 speed_;
	S16 tail_;
	Technic::DIRECTION edge_;

	TecPIDTrace pid_trace_;
	LocalState local_status_;
};

//Mission_Default Mission_Default::instance_;
