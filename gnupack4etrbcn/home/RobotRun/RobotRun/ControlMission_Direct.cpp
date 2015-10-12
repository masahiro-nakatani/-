#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"

class ControlMission_Direct : public Mission {
	
	S16 speed_l_;
	S16 speed_r_;
	S16 tail_;
public:

    ControlMission_Direct(S16 speed_l, S16 speed_r, S16 tail) 
		: speed_l_(speed_l)
		, speed_r_(speed_r)
		, tail_(tail)
	{
	}

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param1 = speed_l_;
		cmd.param2 = speed_r_;
		cmd.param3 = tail_;
		return true;
    }
};
