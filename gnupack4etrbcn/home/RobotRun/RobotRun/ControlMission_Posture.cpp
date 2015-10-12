#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"

class ControlMission_Posture : public Mission {
	
	int from_;
	int to_;
	int step_;
	int interval_;
	bool increment_;
public:

    ControlMission_Posture(int from, int to, int step, int interval) 
		: from_(from)
		, to_(to)
		, step_(step)
		, interval_(interval)
		, increment_(from<to)
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		timer_start(interval_);
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param3 = from_;

		if(to_==from_) return true;

		if(is_timeup())
		{
			timer_start(interval_);
			if(increment_) from_+=step_;
			else from_-=step_;
		}
		
		return true;
    }
};
