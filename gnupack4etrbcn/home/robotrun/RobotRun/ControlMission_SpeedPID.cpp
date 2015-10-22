#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"

class ControlMission_SpeedPID : public Mission {

	TecPIDTrace pid_trace_;
	int from_;
	int to_;
	int step_;
	int interval_;
	bool increment_;
	Technic::DIRECTION edge_;
public:

	ControlMission_SpeedPID(int from, int to_, int step, int interval, Technic::DIRECTION edge = Technic::LEFT)
		: from_(from)
		, to_(to_)
		, step_(step)
		, interval_(interval)
		, increment_(from<to_)
		, edge_(edge)
	{
	}

	virtual void Init(RobotInfo ri, NavInfo ni)
	{
		timer_start(interval_);
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd)
	{
		cmd = pid_trace_.Calclate(ri.light_sensor_val, from_, edge_);

		if (to_ == from_) return true;

		if (is_timeup())
		{
			timer_start(interval_);
			if (to_ != from_)
			{
				if (increment_) from_ += step_;
				else from_ -= step_;
			}
		}

		return true;
	}
};
