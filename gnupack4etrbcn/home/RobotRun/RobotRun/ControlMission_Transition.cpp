class ControlMission_Transition : public Mission {
	
	Mission *posture_;
	Mission *speed_;

public:

    ControlMission_Transition(Mission *posture,  Mission *speed)
		: posture_(posture)
		, speed_(speed)
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
		return true;
    }
};
