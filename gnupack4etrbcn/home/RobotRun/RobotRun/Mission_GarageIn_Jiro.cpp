#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

class Mission_GarageIn : public Mission {
private:
	enum LocalState {
		UNTIL_MARKER,
		GARAGE_IN,
		STOP1,
		STOP2,
	};

public:

    Mission_GarageIn() : Mission() {
		if (CourseInfo::type == CourseInfo::InCourse) {
			edge_ = Technic::RIGHT;
			score_ = MarioTimeUp;
		} else {
			edge_ = Technic::LEFT;
			score_ = CsikosPost;
		}
	}

    virtual void Init(RobotInfo ri, NavInfo ni){
		tec_pid_.Init();
        tec_s_.Init(ri);
		tec_r_.Init();
		TecMusic::Instance().StartMusic(score_);
		is_state_move_ = true;
		speed_ = 80;
		local_status_ = UNTIL_MARKER;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		switch (local_status_) {
		case UNTIL_MARKER:		// マーカー検知待ち
#if 1
			if (is_state_move_) {
				is_state_move_ = false;
			}
			cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = 0;
			if (evf.is_SetEvent(Robot::EV_MARKER)) {
				is_state_move_ = true;
				local_status_ = GARAGE_IN;
//				Robot::Instance().Beep(Robot::A2);
			}
#else		// デバッグ用(マーカー検知の代わりにタイマタイムとする)
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(2000);
			}
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			cmd.param3 = 0;
			if (is_timeup()) {
				is_state_move_ = true;
				local_status_ = GARAGE_IN;
//				Robot::Instance().Beep(Robot::A2);
			}
#endif
			break;

		case GARAGE_IN:			// ガレージ入庫
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(1000);
			}
			speed_ = 10;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			cmd.param3 = 100;			// しっぽ下ろす
			if(is_timeup()){
				is_state_move_ = true;
				local_status_ = STOP1;
//				Robot::Instance().Beep(Robot::B);
			}
			break;

		case STOP1:				// ガレージ停車(少し後進)
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(1000);
			}
			speed_ = -10;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			cmd.param3 = 100;
			if(is_timeup()){
				is_state_move_ = true;
				local_status_ = STOP2;
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
	U8 *score_;

    LocalState local_status_;

};
