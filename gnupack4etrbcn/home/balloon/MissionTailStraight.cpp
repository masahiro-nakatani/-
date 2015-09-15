#include "Mission.h"
#include "Technic.h"

#define TAIL_ANGLE			 60

class Mission_TailStraight : public Mission {
public:

    Mission_TailStraight(S32 timer = 0, S16 speed = 125) : Mission() {
		timer_ = timer;
		speed_ = speed;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		if (timer_ > 0) {
			TimerStart(timer_);
		}
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
		signed int angle = TAIL_ANGLE;

		// 倒立APIを使わず、直接PWM値をセット
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param1 = 70;
		cmd.param2 = 70;
		cmd.param3 = angle;

		if (IsTimeup()) {
			ret = false;
			Robot::Instance().Beep();
		}

        return ret;

    }



private:

	S32 timer_;
	S16 speed_;

};
