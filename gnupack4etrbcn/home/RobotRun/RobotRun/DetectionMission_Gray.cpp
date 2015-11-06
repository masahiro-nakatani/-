#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Gray : public Mission {
	int gray_count_;
	int count_max_;
	int sampling_count;
	bool black;
	bool white;
	bool sampling_counter;
public:
	DetectionMission_Gray(int count_max)
	{
		count_max_ = count_max;
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		gray_count_ = 0;
		black = false;
		white = true;
		sampling_counter = false;
		sampling_count = 0;
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd) 
	{
		/*S16 valW = CourseInfo::colorWhite - ri.light_sensor_val;
		if (valW < 0) valW = -valW;
		S16 valG = CourseInfo::colorGray - ri.light_sensor_val;
		if (valG < 0) valG = -valG;
		S16 valB = CourseInfo::colorBlack - ri.light_sensor_val;
		if (valB < 0) valB = -valB;*/

		S16 valHighLange = CourseInfo::colorGray+5; 
		S16 valLowLange = CourseInfo::colorGray-30;

		if(ri.light_sensor_val > 600)
		{
			Robot::Instance().Beep();
			return false;
		}


		/*if(valHighLange > ri.light_sensor_val && ri.light_sensor_val > valLowLange)
		{
			sampling_counter = true;
		}

		if(sampling_counter)
		{
			++sampling_count;
			if(valHighLange > ri.light_sensor_val && ri.light_sensor_val > valLowLange)
			{
				++gray_count_;
			}

			if(sampling_count > 100){
				if(gray_count_ > 0)
				{
					Robot::Instance().Beep();
					return false;
				}
				else {
					gray_count_ = 0;
					sampling_counter = false;
				}
			}
		}*/

		/*if (ri.light_sensor_val > 660)
		{
			black = true;
		}
		if (ri.light_sensor_val < valUnder)
		{
			white = true && white;
		}
		else
		{
			white=false;

		}
		++sampling_count;

		if (sampling_count > 1000)
		{
			sampling_count = 0;
			if (!black && !white)
			{
				Robot::Instance().Beep();
				return false;
			}

			black = false;
		}*/

		/*if (valG<valB && valG<valW)
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
		}*/
		return true;
	}
};
