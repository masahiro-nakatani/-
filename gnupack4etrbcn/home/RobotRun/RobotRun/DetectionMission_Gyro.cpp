#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Gyro : public Mission{
	S16 std_;
	S16 max_;
public:
	DetectionMission_Gyro(S16 std, S16 max)
		:max_(max)		
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		std_ = ri.gyro_sensor_val;
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd) {

		S16 val = ri.gyro_sensor_val - std_;
		if (val < -max_ || max_ < val)
		{
			Robot::Instance().Beep();
			return false;
		}
		return true;
	}
};
