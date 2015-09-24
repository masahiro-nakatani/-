#include "Mission.h"
#include "Technic.h"

class Mission_PosBased_Straight : public Mission {
	S16 speed_;
	TecBezierTrace bezier_;
	F64 goal_x_;
	F64 goal_y_;
	bool finished_;

	static const Vec patternRR_[]; // �E�E
	static const S16 patternRR_num_;
	static const S32 patternRR_time_[];

public:

    Mission_PosBased_Straight(S16 speed, F64 goal_x, F64 goal_y) 
		: speed_(speed)
		, goal_x_(goal_x)
		, goal_y_(goal_y)
		, finished_(false)
	{
    }

    virtual void Init(RobotInfo ri, NavInfo ni){

		bezier_.Init(ni,patternRR_,patternRR_time_, patternRR_num_);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		// �|��API���g�킸�A����PWM�l���Z�b�g
		cmd.Mode = RobotCmd::DIRECT_MODE;
		cmd.param1 = speed_/2;
		cmd.param2 = speed_;
		cmd.param3 = RobotCmd::NO_TAIL_CNTL;

		//cmd = bezier_.Calclate(ni.pos);

		
		if(ni.pos.Y > goal_y_)
		{
			if(!finished_)
			{
				cmd.param1 = 0;
				cmd.param2 = 0;
				finished_ = true;
			}
			else
			{
				Robot::Instance().Beep(Robot::C);
				return false;
			}
		}
        return true;

    }
};

const Vec Mission_PosBased_Straight::patternRR_[] = { // �E�E
	Vec(0, 0, true), Vec(0, 100, true), Vec(0, 200, true),
};
const S16 Mission_PosBased_Straight::patternRR_num_ = sizeof Mission_PosBased_Straight::patternRR_ / sizeof Mission_PosBased_Straight::patternRR_[0];
const S32 Mission_PosBased_Straight::patternRR_time_[] = {
	2000// [msec]
};
