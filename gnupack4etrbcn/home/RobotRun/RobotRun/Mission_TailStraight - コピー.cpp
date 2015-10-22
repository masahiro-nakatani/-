#include "Mission.h"
#include "Technic.h"
#include "ControlMission_PID.cpp"
#include "ControlMission_Speed.cpp"
#include "ControlMission_Posture.cpp"
#include "DetectionMission_Time.cpp"
#include "DetectionMission_Sonar.cpp"
#include "ControlMission_Transition.cpp"

class Mission_TailStraight : public Mission {
	enum
	{ 
		STAGE_STRAIGHT_TO_GATE = 0,
		STAGE_STOP_BEFORE_TILT_UNDER,
		STAGE_TILT_UNDER,
		STAGE_PASSING_GATE,
		STAGE_STOP_BEFORE_TILT_UPPER,
		STAGE_TILT_UPPER,
		STAGE_STRAIGHT_TO_GOAL,
		MISSION_COUNT = STAGE_STRAIGHT_TO_GOAL + 1 
	};
	ControlMission_PID run_straight_slow_;
	ControlMission_Transition stop_;
	ControlMission_Transition tilt_under_;
	ControlMission_Transition tilt_upper_;
	ControlMission_Posture tilt_down_;
	DetectionMission_Sonar sonar_mission_;
	DetectionMission_Time timed_mission_10;
	DetectionMission_Time timed_mission_100;
	DetectionMission_Time timed_mission_1000;
	DetectionMission_Time timed_mission_3000;
	Mission* p_control_missions_[MISSION_COUNT];
	Mission* p_detection_mission_[MISSION_COUNT];
	int mission_index_;
	int mission_count_;

	static ControlMission_Speed* zero_speed(){ return new ControlMission_Speed(0,0,0,0,10,0); }
	static ControlMission_Speed* slow_speed(){ return new ControlMission_Speed(0,0,10,10,1,1); }
	static ControlMission_Posture* no_posture(){ return new ControlMission_Posture(RobotCmd::NO_TAIL_CNTL,RobotCmd::NO_TAIL_CNTL,0,0,0); }

public:

    Mission_TailStraight(S32 timer = 0, S16 speed = 125) 
		: run_straight_slow_(30,RobotCmd::NO_TAIL_CNTL)
		, stop_(no_posture(), new ControlMission_Speed(30,30,0,0,1,10))
		, tilt_under_(new ControlMission_Posture(80,40,1,100,10),zero_speed())
		, tilt_upper_(new ControlMission_Posture(90,100,1,100, -20),zero_speed())
		, tilt_down_(80,40,1,100,10)
		, sonar_mission_(20)
		, timed_mission_10(10)
		, timed_mission_100(100)
		, timed_mission_1000(1000)
		, timed_mission_3000(3000)
		, mission_index_(0)
		, mission_count_( sizeof(p_control_missions_)/sizeof(p_control_missions_[0]) )
	{
		p_control_missions_[STAGE_STRAIGHT_TO_GATE] = &run_straight_slow_;
		p_detection_mission_[STAGE_STRAIGHT_TO_GATE] = &sonar_mission_;

		p_control_missions_[STAGE_TILT_UNDER] = &tilt_under_;
		p_detection_mission_[STAGE_TILT_UNDER] = &timed_mission_10;

		//p_control_missions_[STAGE_TILT_UNDER] = &tilt_down_;
		//p_detection_mission_[STAGE_TILT_UNDER] = &timed_mission_100;
		
		p_control_missions_[STAGE_STOP_BEFORE_TILT_UNDER] = &stop_;
		p_detection_mission_[STAGE_STOP_BEFORE_TILT_UNDER] = &timed_mission_100;
 
		p_control_missions_[STAGE_PASSING_GATE] = &run_straight_slow_;
		p_detection_mission_[STAGE_PASSING_GATE] = &timed_mission_3000;

		p_control_missions_[STAGE_STOP_BEFORE_TILT_UPPER] = &stop_;
		p_detection_mission_[STAGE_STOP_BEFORE_TILT_UPPER] = &timed_mission_1000;

		p_control_missions_[STAGE_TILT_UPPER] = &tilt_upper_;
		p_detection_mission_[STAGE_TILT_UPPER] = &timed_mission_3000;

		p_control_missions_[STAGE_STRAIGHT_TO_GOAL] = &run_straight_slow_;
		p_detection_mission_[STAGE_STRAIGHT_TO_GOAL] = &timed_mission_3000;
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
