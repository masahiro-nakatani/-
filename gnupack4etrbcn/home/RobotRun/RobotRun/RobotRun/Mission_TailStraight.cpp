#include "Mission.h"
#include "Technic.h"

#define TAIL_ANGLE			 60

class Mission_TailStraight : public Mission {
public:

    Mission_TailStraight(S32 timer = 0, S16 speed = 125) 
		: timer_(timer)
		, speed_(speed)
	{
		timer_ = timer;
		speed_ = speed;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		if (timer_ > 0) {
			timer_start(timer_);
		}
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		// 倒立APIを使わず、直接PWM値をセット
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param1 = 50;
		cmd.param2 = 50;
		cmd.param3 = TAIL_ANGLE;

		if (is_timeup()) {
			Robot::Instance().Beep(Robot::G);
			return false;
		}

        return true;

    }



private:

	S32 timer_;
	S16 speed_;

};
