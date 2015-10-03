
#include "Mission.h"
#include "Technic.h"


class Mission_Decoding : public Mission {
private:
	enum LocalState{
		MOVE = 0,
		DECODE,
		PID,
		END
	};

public:

	Mission_Decoding(void) : Mission() {
	}

	virtual void Init(RobotInfo ri, NavInfo ni){
		change_state(MOVE);
		turn_speed_ = 30;
	}


	virtual bool Run(RobotInfo ri, NavInfo nii, EventFlag evf, RobotCmd& cmd ){

		NavInfo ni;
		ni.pos = nii.pos;

		bool ret = true;

		switch(status_){
		case MOVE:					// マーカまで移動中
			// ステータス変化時動作
			if ( is_state_move_ ){
				tec_pid_.Init();
				is_state_move_ = false;
				posBase = nii.pos;
			}

			if ( (posBase.X - ni.pos.X) > 150 ){
				Robot::Instance().Beep();
				change_state(DECODE);
			} else {
				cmd = tec_pid_.Calclate(ri.light_sensor_val, 50, Technic::RIGHT);
			}
			break;

		case DECODE:				// ペットボトル検知中
			// ステータス変化時動作
			if ( is_state_move_ ){
				tec_r_.Init();
				is_state_move_ = false;
				posBase = nii.pos;
				turn_speed_ = -30;
				cmd = tec_r_.Calclate(0,0,Technic::RIGHT);
			} else {
				cmd = tec_r_.Calclate(0,turn_speed_,Technic::RIGHT);
			}

			if ( evf.is_SetEvent(Robot::EV_SONAR_SENSOR_ON) ){
				Robot::Instance().Beep();
				CourseInfo::PetBottle1 = true;
				change_state(PID);
				cmd = tec_r_.Calclate(0,0,Technic::RIGHT);
			}

			if( (nii.pos.Ang - posBase.Ang) > 0.75 * M_PI ){
				// 135度回ってペットボトルが検知できない場合
				// 2の位置にペットボトルありと判断する
				Robot::Instance().Beep();
				CourseInfo::PetBottle2 = true;
				change_state(PID);
				cmd = tec_r_.Calclate(0,0,Technic::RIGHT);
			}

			break;

		case PID:					// PIDトレース復帰中
			if ( is_state_move_ ){
				tec_r_.Init();
				is_state_move_ = false;
				turn_speed_ = 30;
				cmd = tec_r_.Calclate(0,0,Technic::RIGHT);
			} else {
				cmd = tec_r_.Calclate(0,turn_speed_,Technic::RIGHT);
			}

			if( (nii.pos.Ang - posBase.Ang) <= 0 ){
				change_state(END);
				cmd = tec_r_.Calclate(0,0,Technic::RIGHT);
			}

			break;

		case END:					// ミッション終了
			// ステータス変化時動作
			if ( is_state_move_ ){
				tec_pid_.Init();
				is_state_move_ = false;
			}

			cmd = tec_pid_.Calclate(ri.light_sensor_val, 50, Technic::RIGHT);
			ret = false;
			break;
	   }
		cmd.param3 = 0;
		return ret;
	}


	void change_state(LocalState st){
		status_ = st;
		is_state_move_ = true;
	}

private:

	TecPIDTrace tec_pid_;
	TecRotate tec_r_;
	TecStraight tec_s_;

	S16 turn_speed_;

	LocalState status_;

	bool is_state_move_;

	Position posBase;
};

//Mission_Decoding Mission_Decoding::instance_;
