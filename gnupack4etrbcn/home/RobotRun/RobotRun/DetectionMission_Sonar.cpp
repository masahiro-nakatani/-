#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Sonar : public Mission 
{
	
	int receive_count_;
	S16 distance_;
public:
    DetectionMission_Sonar(S16 distance) 
		: receive_count_(0)
		, distance_(distance)
	{
	}

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		if(Robot::Instance().sonar_distance < distance_ )
		{
				receive_count_++;
		}
		if(receive_count_ > 3)
		{
			Robot::Instance().Beep();
			return false;
		}
		return true;
    }

};
