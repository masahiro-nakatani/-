#include "ControlMission_Posture.cpp"
#include "ControlMission_Speed.cpp"

class ControlMission_Transition : public Mission {
	
	ControlMission_Posture posture_;
	ControlMission_Speed speed_;
public:

    ControlMission_Transition(const ControlMission_Posture& posture, const ControlMission_Speed& speed) 
		: posture_(posture)
		, speed_(speed)
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		posture_.Init(ri,ni);
		speed_.Init(ri,ni);
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		posture_.Run(ri,ni,evf,cmd);
		speed_.Run(ri,ni,evf,cmd);
		return true;
    }
};
