#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class ControlMission_Transition : public Mission {
	
	Mission *posture_;
	Mission *speed_;
	RobotCmd::DRIVE_MODE mode_;

public:

    ControlMission_Transition(Mission *posture,  Mission *speed, RobotCmd::DRIVE_MODE mode)
		: posture_(posture)
		, speed_(speed)
		, mode_(mode)
	{
	}

	~ControlMission_Transition()
	{
		delete posture_;
		delete speed_;
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		posture_->Init(ri,ni);
		speed_->Init(ri,ni);
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		posture_->Run(ri,ni,evf,cmd);
		speed_->Run(ri,ni,evf,cmd);
		cmd.Mode = mode_;
		return true;
    }
};
