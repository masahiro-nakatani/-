#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class Mission_Default : public Mission {
public:

    Mission_Default(S32 timer = 0, S16 speed = 60, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		if (timer_ > 0) {
			TimerStart(timer_);
		}
		//Navigator::Instance().StartDirCorrection (400.0, 180.0);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

        cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, Technic::RIGHT);
		cmd.param3 = tail_;
		if (IsTimeup()) {
			ret = false;
			Robot::Instance().Beep();
		}

        return ret;

    }



private:

    TecPIDTrace pid_trace_;
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_Default instance_;

};

//Mission_Default Mission_Default::instance_;
