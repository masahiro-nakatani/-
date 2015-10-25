#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"

class ControlMission_Calibration : public Mission {

public:

	ControlMission_Calibration()
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd)
	{

		Robot::Instance().Calibration();
		cmd.param1 = 0;
		cmd.param2 = 0;

		return true;
	}
};
