#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class MissionKaidan : public Mission {
private:
private:
	enum LocalState{
		STAIRS_PRE_IN = 1,	 	// 階段衝突前
		STAIRS_IN,			 	// 階段衝突前

		STAIRS_HIT_ONE,		// 片輪乗り上げ中
		STAIRS_STABLE,		// 安定化
		STAIRS_FLATTEN,		// 安定化
		STAIRS_FORWARD,		// ゆっくり前進

		STAIRS_HIT_ONE2,	// 片輪乗り上げ中
		STAIRS_STABLE2,		// 安定化
		STAIRS_FLATTEN2,	// 安定化
		STAIRS_FORWARD2,	// ゆっくり前進

		STAIRS_COURNER,		// コーナ検知

		STAIRS_OUT,			// 階段から落下(ライン復帰)
		STAIRS_END			// 流し走行
	};

public:

	MissionKaidan(void) : Mission() {
	}

	virtual void Init(RobotInfo ri, NavInfo ni){
		speed_ = 120;
		local_status_ = STAIRS_PRE_IN;
		is_state_move_ = true;
		tec_pid_.Init();
		flag = 0;
		start_encoderL = start_encoderR = 0;
		flatten_count = 0;
		TimerStart(1000);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		bool ret = true;

		int uplightValue = 0;
		bool bd = false;

		switch (local_status_) {

			// 流し走行中(階段衝突前)
			// do: PIDライントレース
			// exit: ジャイロ
			case STAIRS_PRE_IN:
				//cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(speed_ * 1.0), Technic::RIGHT);
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(80), Technic::RIGHT);

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::C);	//ド
					local_status_ = STAIRS_IN;
				}
				break;

			// 流し走行中(階段衝突前
			// do: PIDライントレース
			// exit: ジャイロ
			case STAIRS_IN:
				//Robot::Instance().SetGyro(0);
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(40), Technic::RIGHT);
				cmd.param3 = 0;

				if(evf.is_SetEvent(Robot::EVENT_GYRO)){
					Robot::Instance().Beep(Robot::D);	//レ
					local_status_ = STAIRS_HIT_ONE;
					TimerStart(250);
				}
				break;

			case STAIRS_HIT_ONE:
				cmd = tec_s_.Calclate(100, ri.rot_encoderL_val, ri.rot_encoderR_val);

				Robot::Instance().Beep(Robot::E);	//ミ
				if(IsTimeup())
				{
					local_status_ = STAIRS_STABLE;
					TimerStart(1000);
				}
				break;
			// 安定化
			//
			case STAIRS_STABLE:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);

				if(IsTimeup())
				{
					local_status_ = STAIRS_FLATTEN;
				}
				break;

			// 安定化中
			// do: 旋回走行
			// exit: 480ms
			case STAIRS_FLATTEN:
				flatten_count++;
				//if(ri.light_sensor_val < CourseInfo::colorBlack+20){
				if(flatten_count > 20){
					
					local_status_ = STAIRS_FORWARD;
				//	DEBUG_PRINT2("sd", "STAIRS_N ", local_status_);
					TimerStart(1000);
					tec_s_.Init(ri);
				}
				break;

			// ゆっくり前進
			case STAIRS_FORWARD:
				//cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);
				DEBUG_PRINT1("s", "STAIRS_FORWARD");
				cmd = tec_pid_.Calclate2(ri.light_sensor_val, (S16)(20), Technic::RIGHT);
				Robot::Instance().Beep(Robot::G);	//ソ

				if(IsTimeup() && evf.is_SetEvent(Robot::EVENT_GYRO)){
					Robot::Instance().Beep(Robot::A);	//ラ
					local_status_ = STAIRS_HIT_ONE2;
					TimerStart(250);
				}
				break;
			
			case STAIRS_HIT_ONE2:
				cmd = tec_s_.Calclate(100, ri.rot_encoderL_val, ri.rot_encoderR_val);
				Robot::Instance().Beep(Robot::E);	//ミ
				if(IsTimeup())
				{
					local_status_ = STAIRS_STABLE2;
					TimerStart(1000);
				}
				break;

			// 安定化
			//
			case STAIRS_STABLE2:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);

				if(IsTimeup())
				{
					flatten_count = 0;
					local_status_ = STAIRS_FLATTEN2;
				}
				break;

			// 安定化中
			case STAIRS_FLATTEN2:
				flatten_count++;
				if(flatten_count > 20){
					Robot::Instance().Beep(Robot::C);	//ド
					local_status_ = STAIRS_FORWARD2;
					tec_s_.Init(ri);
					TimerStart(1000);
					flatten_count = 0;
				}

				break;

			// ゆっくり前進
			case STAIRS_FORWARD2:
				DEBUG_PRINT1("s", "STAIRS_FORWARD2");
				cmd = tec_pid_.Calclate2(ri.light_sensor_val, (S16)(10), Technic::RIGHT);

				int min = CourseInfo::colorGray * 0.95;
				int max = CourseInfo::colorGray * 1.05;

				if(ri.light_sensor_val >= min && 
				   ri.light_sensor_val <= max)
				{
					flatten_count ++;
				}
				else
				{
					flatten_count = 0;
				}

				if(IsTimeup() && flatten_count > 300){
				//if(IsTimeup() && evf.is_SetEvent(Robot::EVENT_GYRO)){
					Robot::Instance().Beep(Robot::D);	//レ
					local_status_ = STAIRS_COURNER;
					base_ang_ = ni.pos.Ang;
				}
				break;

			// カーブ位置検知
			case STAIRS_COURNER:
				DEBUG_PRINT2("sd", "STAIRS_COURNER ", ni.pos.Ang);
				// 90度カーブ

                cmd.param1 = 0;
                cmd.param2 = -50;
                /* 終了条件 */
                if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
                    local_status_ = STAIRS_OUT;
					TimerStart(10000);
                }
				break;

			// ライン復帰
			case STAIRS_OUT:
				cmd = tec_pid_.Calclate2(ri.light_sensor_val, (S16)(20), Technic::RIGHT);
				//cmd = tec_s_.Calclate(20, ri.rot_encoderL_val, ri.rot_encoderR_val);
				DEBUG_PRINT1("s", "STAIRS_OUT");
				if(IsTimeup()){
					Robot::Instance().Beep(Robot::E);	//ミ
					local_status_ = STAIRS_END;
				}
				break;
			default:
				ret = false;
				break;
		}
		
		
		return ret;
	}


	virtual char* Name(void){
		return "Mission_Stairs";
	}

private:

	S16 speed_;
	S16 rotation_;
	F32 angSum;
	S32 angNum;
	F32 angBase;
	bool is_state_move_;
	TecStraight tec_s_;
	TecPIDTrace tec_pid_;
	TecRotate tec_r_;
	LocalState local_status_;
	int flag;
	S32 start_encoderR;
	S32 start_encoderL;
	int flatten_count;

	F64 base_ang_;
};
