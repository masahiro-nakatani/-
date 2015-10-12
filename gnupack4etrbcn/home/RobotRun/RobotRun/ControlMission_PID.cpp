#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

class ControlMission_PID : public Mission {
	
	TecPIDTrace pid_trace_;
	Technic::DIRECTION edge_;
	S16 speed_;
	S16 tail_;
public:

    ControlMission_PID(S16 speed, S16 tail, Technic::DIRECTION edge = Technic::LEFT) 
		: edge_(edge)
		, speed_(speed)
		, tail_(tail)
	{
	}


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{
		cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
		cmd.param3 = tail_;
		return false;
    }
};
