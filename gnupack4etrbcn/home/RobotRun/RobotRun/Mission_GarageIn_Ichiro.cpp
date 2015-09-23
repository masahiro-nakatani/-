#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

class Mission_GarageIn : public Mission {
private:
	enum LocalState {
		TIMER_RUN,
		UNTIL_MARKER,
		GARAGE_IN,
		STOP1,
		STOP2,
	};

public:

    Mission_GarageIn() : Mission() {
		if (CourseInfo::type == CourseInfo::InCourse) {
			edge_ = Technic::LEFT;
		} else {
			edge_ = Technic::RIGHT;
		}
	}

    virtual void Init(RobotInfo ri, NavInfo ni){
		tec_pid_.Init();
        tec_s_.Init(ri);
		tec_r_.Init();
		is_state_move_ = true;
		speed_ = 70;
		local_status_ = TIMER_RUN;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		switch (local_status_) {
		case TIMER_RUN:			// タイムアップ待ち
			if (is_state_move_) {
				is_state_move_ = false;
				tec_pid_.Init();
				timer_start(2000);
			}
			cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = 0;
			if (is_timeup()) {
				is_state_move_ = true;
				local_status_ = UNTIL_MARKER;
				Robot::Instance().Beep(Robot::C);
			}
			break;

		case UNTIL_MARKER:		// マーカー検知待ち
			if (is_state_move_) {
				is_state_move_ = false;
				tec_pid_.Init();
			}
			cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = 0;
			if (evf.is_SetEvent(Robot::EV_MARKER)) {
				is_state_move_ = true;
				speed_ = 0;
				local_status_ = GARAGE_IN;
				Robot::Instance().Beep(Robot::D);
			}
			break;

		case GARAGE_IN:			// ガレージ入庫
			if (is_state_move_) {
				is_state_move_ = false;
				tec_s_.Init(ri);
				timer_start(1000);
			}
			speed_ = 15;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			cmd.param3 = 100;			// しっぽ下ろす
			if(is_timeup()){
				is_state_move_ = true;
				local_status_ = STOP1;
				Robot::Instance().Beep(Robot::E);
			}
			break;

		case STOP1:				// ガレージ停車(少し後進)
			if (is_state_move_) {
				is_state_move_ = false;
				tec_s_.Init(ri);
				timer_start(500);
			}
			speed_ = -15;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			cmd.param3 = 100;
			if(is_timeup()){
				is_state_move_ = true;
				local_status_ = STOP2;
				Robot::Instance().Beep(Robot::F);
			}
			break;

		case STOP2:				// ガレージ停車(完全停止)
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 0;
			cmd.param2 = 0;
			cmd.param3 = 100;
			break;

		default:
			ret = false;
			break;
		}

        return ret;
    }

private:

    TecPIDTrace    tec_pid_;
    TecStraight    tec_s_;
	TecRotate      tec_r_;
	CourseInfo::CourseType type_;
	Technic::DIRECTION edge_;
	bool is_state_move_;
	S16 speed_;

    LocalState local_status_;

};
