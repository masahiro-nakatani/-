#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Gray : public Mission {

public:
	DetectionMission_Gray()
	{
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd) 
	{
		S16 valW = CourseInfo::colorWhite - ri.light_sensor_val;
		if (valW < 0) valW = -valW;
		S16 valG = CourseInfo::colorGray - ri.light_sensor_val;
		if (valG < 0) valG = -valG;
		S16 valB = CourseInfo::colorBlack - ri.light_sensor_val;
		if (valB < 0) valB = -valB;

		if (valG<valW && valG<valB)
		{
			S16 temp = CourseInfo::colorBlack;
			CourseInfo::colorBlack = CourseInfo::colorGray;
			CourseInfo::colorGray = temp;
			Robot::Instance().Beep();
			return false;
		}
		return true;
	}
};
