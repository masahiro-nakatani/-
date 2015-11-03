#include "Mission.h"
#include "Technic.h"
#include "ControlMission_SpeedPID.cpp"
#include "ControlMission_Speed.cpp"
#include "ControlMission_Posture.cpp"
#include "DetectionMission_Time.cpp"
#include "ControlMission_Transition.cpp"


class Mission_Footrace : public Mission {
	enum
	{ 
		MISSION_COUNT = 1
	};
	TecPIDTrace pid_trace_;
	ControlMission_Transition run_straight_midle_;
	DetectionMission_Time timed_mission_100000;
	Mission* p_control_missions_[MISSION_COUNT];
	Mission* p_detection_mission_[MISSION_COUNT];
	int mission_index_;
	int mission_count_;

public:

    Mission_Footrace() 
		: run_straight_midle_(new ControlMission_Posture(RobotCmd::NO_TAIL_CNTL,RobotCmd::NO_TAIL_CNTL,0,0), new ControlMission_SpeedPID(&pid_trace_, 20,70, 1, 100), RobotCmd::NORMAL_MODE)
		, timed_mission_100000(100000)
		, mission_index_(0)
		, mission_count_( sizeof(p_control_missions_)/sizeof(p_control_missions_[0]) )
	{
		p_control_missions_[0] = &run_straight_midle_;
		p_detection_mission_[0] = &timed_mission_100000;
		
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		p_control_missions_[0]->Init(ri,ni);
		p_detection_mission_[0]->Init(ri,ni);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		if(p_detection_mission_[mission_index_]->Run(ri,ni,evf,cmd))
		{
			p_control_missions_[mission_index_]->Run(ri,ni,evf,cmd);
		}
		else
		{
			++mission_index_;
			if(mission_index_>=mission_count_) return false;
			p_control_missions_[mission_index_]->Init(ri,ni);
			p_detection_mission_[mission_index_]->Init(ri,ni);
		}
		return true;
    }
};