#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Gray : public Mission {
	int gray_count_;
	int count_max_;
public:
	DetectionMission_Gray(int count_max)
	{
		count_max_ = count_max;
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		gray_count_ = 0;

	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd) 
	{
		S16 valW = CourseInfo::colorWhite - ri.light_sensor_val;
		if (valW < 0) valW = -valW;
		S16 valG = CourseInfo::colorGray - ri.light_sensor_val;
		if (valG < 0) valG = -valG;
		S16 valB = CourseInfo::colorBlack - ri.light_sensor_val;
		if (valB < 0) valB = -valB;

		if (valG<valB && valG<valW)
		{
			gray_count_++;
		}
		else if(valG > valB)
		{
			gray_count_ = 0;
		}

		if (gray_count_ > count_max_)
		{
			Robot::Instance().Beep();
			return false;
		}
		return true;
	}
};
