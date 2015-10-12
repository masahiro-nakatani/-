#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Time : public Mission 
{
	S32 timer_;
public:
    DetectionMission_Time(S32 timer) 
		: timer_(timer)
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		if (timer_ > 0) 
		{
			timer_start(timer_);
		}
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		if (is_timeup())
		{
			Robot::Instance().Beep(Robot::G);
			return false;
		}

        return true;
    }

};
