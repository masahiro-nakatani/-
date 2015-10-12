#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"

class ControlMission_Speed : public Mission {
	
	int l_from_, r_from_;
	int l_to_, r_to_;
	int step_;
	int interval_;
	bool l_increment_, r_increment_;
public:

    ControlMission_Speed(int l_from, int r_from, int l_to, int r_to, int step, int interval) 
		: l_from_(l_from)
		, r_from_(r_from)
		, l_to_(l_to)
		, r_to_(r_to)
		, step_(step)
		, interval_(interval)
		, l_increment_(l_from<l_to)
		, r_increment_(r_from<r_to)
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		timer_start(interval_);
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param1 = l_from_;
		cmd.param2 = r_from_;

		if(l_to_==l_from_ && r_to_ == r_from_) return true;

		if(is_timeup())
		{
			timer_start(interval_);
			if(l_to_!=l_from_)
			{
				if(l_increment_) l_from_+=step_;
				else l_from_-=step_;
			}
			if(r_to_!=r_from_)
			{
				if(r_increment_) r_from_+=step_;
				else r_from_-=step_;
			}
		}
		
		return true;
    }
};
