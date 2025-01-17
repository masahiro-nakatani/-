#include "Mission.h"
#include "Technic.h"
#include "ControlMission_PID.cpp"
#include "ControlMission_SpeedPID.cpp"
#include "ControlMission_Speed.cpp"
#include "ControlMission_Posture.cpp"
#include "DetectionMission_Time.cpp"
#include "DetectionMission_Sonar.cpp"
#include "DetectionMission_Endless.cpp"
#include "ControlMission_Transition.cpp"


class Mission_TailStraight : public Mission {
	enum
	{ 
		MISSION_COUNT = 8
	};
	TecPIDTrace pid_trace_;
	ControlMission_SpeedPID run_straight_slow_;
	ControlMission_Transition run_straight_midle_;
	ControlMission_Transition run_straight_start_;
	ControlMission_Transition run_direct_;
	ControlMission_Transition run_direct_goal_;
	ControlMission_Transition stop_;
	ControlMission_Transition tilt_under_;
	ControlMission_Transition tilt_upper_;
	DetectionMission_Sonar sonar_mission_;
	DetectionMission_Time timed_mission_500;
	DetectionMission_Time timed_mission_1000;
	DetectionMission_Time timed_mission_3000;
	DetectionMission_Time timed_mission_4000;
	DetectionMission_Time timed_mission_5000;
	DetectionMission_Time timed_mission_10000;
	DetectionMission_Endless endless_mission;
	Mission* p_control_missions_[MISSION_COUNT];
	Mission* p_detection_mission_[MISSION_COUNT];
	int mission_index_;
	int mission_count_;

	static ControlMission_Speed* zero_speed(){ return new ControlMission_Speed(0,0,0,0,0,0); }
	static ControlMission_Posture* no_posture(){ return new ControlMission_Posture(RobotCmd::NO_TAIL_CNTL,RobotCmd::NO_TAIL_CNTL,0,0); }

public:

    Mission_TailStraight(S32 timer = 0, S16 speed = 125) 
		: run_straight_slow_(&pid_trace_, 10,40, 1, 100)
		, run_straight_midle_(new ControlMission_Posture(0,0,0,0), new ControlMission_SpeedPID(&pid_trace_, 50,20, 1, 100), RobotCmd::NORMAL_MODE)
		, run_straight_start_(new ControlMission_Posture(0,0,0,0), new ControlMission_SpeedPID(&pid_trace_, 50,50, 1, 100), RobotCmd::NORMAL_MODE)
		, run_direct_(no_posture(),new ControlMission_Speed(20, 20, 20, 20, 1, 100), RobotCmd::DIRECT_MODE )
		, run_direct_goal_(no_posture(),new ControlMission_Speed(80, 80, 20, 20, 1, 100), RobotCmd::DIRECT_MODE )
		, stop_(no_posture(), new ControlMission_Speed(30,30,0,0,1,10), RobotCmd::DIRECT_MODE)
		, tilt_under_(new ControlMission_Posture(80,70,1,100),zero_speed(), RobotCmd::DIRECT_MODE)
		, tilt_upper_(new ControlMission_Posture(100,110,1,100),zero_speed(), RobotCmd::DIRECT_MODE)
		, sonar_mission_(10)
		, timed_mission_500(500)
		, timed_mission_1000(1000)
		, timed_mission_3000(3000)
		, timed_mission_4000(4000)
		, timed_mission_5000(5000)
		, timed_mission_10000(10000)
		, endless_mission()
		, mission_index_(0)
		, mission_count_( sizeof(p_control_missions_)/sizeof(p_control_missions_[0]) )
	{
		p_control_missions_[0] = &run_straight_start_;
		p_detection_mission_[0] = &timed_mission_5000;

		p_control_missions_[1] = &run_straight_midle_;
		p_detection_mission_[1] = &sonar_mission_;
		
		p_control_missions_[2] = &stop_;
		p_detection_mission_[2] = &timed_mission_500;

		p_control_missions_[3] = &tilt_under_;
		p_detection_mission_[3] = &timed_mission_3000;

		p_control_missions_[4] = &run_direct_;
		p_detection_mission_[4] = &timed_mission_4000;

		p_control_missions_[5] = &stop_;
		p_detection_mission_[5] = &timed_mission_1000;

		p_control_missions_[6] = &tilt_upper_;
		p_detection_mission_[6] = &timed_mission_3000;

		p_control_missions_[7] = &run_direct_goal_;
		p_detection_mission_[7] = &endless_mission;
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
