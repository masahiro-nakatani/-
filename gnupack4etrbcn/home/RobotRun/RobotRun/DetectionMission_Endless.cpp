#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class DetectionMission_Endless : public Mission 
{
public:
    DetectionMission_Endless() 
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        return true;
    }

};
