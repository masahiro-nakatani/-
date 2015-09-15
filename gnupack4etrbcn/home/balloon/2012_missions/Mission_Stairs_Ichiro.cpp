//
// FILE NAME	 :  Mission_Stairs.cpp
//
//				 :2010年難所 階段
//
//┌───┬────┬─────────────────────┬──────┐
//│ 履歴 │  DATE  │			  NOTES							│	SIGN	  │
//├───┼────┼─────────────────────┼──────┤
//│A	  │		│新規作成									│			  │
//├───┼────┼─────────────────────┼──────┤
//│	  │		│											│			  │
//├───┼────┼─────────────────────┼──────┤
//│	  │		│											│			  │
//└───┴────┴─────────────────────┴──────┘
#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"

#define PRIM_SPEED 110//1郎
#define COUNTER1 45//1
//#define PRIM_SPEED 114//2郎
//#define COUNTER1 52//2郎

class Mission_Stairs : public Mission {
private:
	enum LocalState{
		STAIRS_PRE_IN = 0,	 	// 階段衝突前
		STAIRS_IN,			 	// 階段衝突前

		STAIRS_HIT_ONE,		// 片輪乗り上げ中
		STAIRS_HIT_BOTH,	// 両輪乗り上げ中
		STAIRS_FLATTEN,		// 安定化
		STAIRS_FORWARD,		// ゆっくり前進

		STAIRS_HIT_ONE2,	// 片輪乗り上げ中
		STAIRS_HIT_BOTH2,	// 両輪乗り上げ中
		STAIRS_FLATTEN2,	// 安定化
		STAIRS_FORWARD2,	// ゆっくり前進

		STAIRS_OUT,			// 階段から落下(ライン復帰)
		STAIRS_END			// 流し走行
	};


public:

	Mission_Stairs(void) : Mission() {
	}

	virtual void Init(RobotInfo ri, NavInfo ni){
		speed_ = 120;
		local_status_ = STAIRS_PRE_IN;
		is_state_move_ = true;
		tec_pid_.Init();
		flag = 0;
		StairsCounter = 0;
		start_encoderL = start_encoderR = 0;
		flatten_count = 0;
		TimerStart(2500);
	}

	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		bool ret = true;
		S32 diffR;
		S32 diffL;

		switch (local_status_) {

			// 流し走行中(階段衝突前)
			// do: PIDライントレース
			// exit: ジャイロ
			case STAIRS_PRE_IN:
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(80), Technic::RIGHT);

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::A2);
					local_status_ = STAIRS_IN;
				}
				break;

			// 流し走行中(階段衝突前)
			// do: PIDライントレース
			// exit: ジャイロ
			case STAIRS_IN:
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.2), Technic::RIGHT);
				cmd.param3 = 0;

				if(evf.is_SetEvent(Robot::EV_GYRO)){
					start_encoderR = ri.rot_encoderR_val;
					start_encoderL = ri.rot_encoderL_val;
					Robot::Instance().Beep(Robot::D);
					local_status_ = STAIRS_HIT_ONE;
				}
				break;

			// 片輪乗り上げ中
			// do: 旋回走行
			// exit: 340ms
			case STAIRS_HIT_ONE:
				cmd = tec_r_.Calclate(30, 100, Technic::LEFT);
				diffR = ri.rot_encoderR_val - start_encoderR;

				if(diffR > 80){//45:1, 50:2
					Robot::Instance().Beep(Robot::E);
					local_status_ = STAIRS_HIT_BOTH;
				}
				break;

			// もう片輪乗り上げ中
			// do: 旋回走行
			// exit: 480ms
			case STAIRS_HIT_BOTH:
				cmd = tec_r_.Calclate(30, 150, Technic::RIGHT);
				diffL = ri.rot_encoderL_val - start_encoderL;

				if(diffL > 80){//45:1, 50:2]]
					Robot::Instance().Beep(Robot::F);
					local_status_ = STAIRS_FLATTEN;
				}
				break;

			// 安定化中
			// do: 旋回走行
			// exit: 480ms
			case STAIRS_FLATTEN:
				diffR = ri.rot_encoderR_val - start_encoderR;
				diffL = ri.rot_encoderL_val - start_encoderL;

				if(diffR - diffL > 1){
					cmd = tec_r_.Calclate(5, 100, Technic::RIGHT);
				}else if(diffL - diffR > 1){
					cmd = tec_r_.Calclate(5, 100, Technic::LEFT);
				}else{
					cmd.param1 = 5;
					cmd.param2 = 0;
					flatten_count++;
				}

				if(flatten_count > 20){
					Robot::Instance().Beep(Robot::B);
					local_status_ = STAIRS_FORWARD;
					TimerStart(4000);
					tec_s_.Init(ri);
				}

				break;

			// ゆっくり前進
			case STAIRS_FORWARD:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);

				if(IsTimeup() && evf.is_SetEvent(Robot::EV_GYRO)){
					start_encoderR = ri.rot_encoderR_val;
					start_encoderL = ri.rot_encoderL_val;
					Robot::Instance().Beep(Robot::C);
					local_status_ = STAIRS_HIT_ONE2;
				}
				break;


			// 片輪乗り上げ中
			// do: 旋回走行
			case STAIRS_HIT_ONE2:
				cmd = tec_r_.Calclate(30, 100, Technic::RIGHT);
				diffL = ri.rot_encoderL_val - start_encoderL;


				if(diffL > 80){//45:1, 50:2
					Robot::Instance().Beep(Robot::E);
					local_status_ = STAIRS_HIT_BOTH2;
				}
				break;

			// もう片輪乗り上げ中
			// do: 旋回走行
			case STAIRS_HIT_BOTH2:
				cmd = tec_r_.Calclate(30, 150, Technic::LEFT);
				diffR = ri.rot_encoderR_val - start_encoderR;

				if(diffR > 80){//45:1, 50:2]]
					Robot::Instance().Beep(Robot::F);
					local_status_ = STAIRS_FLATTEN2;
					flatten_count = 0;
				}
				break;

			// 安定化中
			case STAIRS_FLATTEN2:
				diffR = ri.rot_encoderR_val - start_encoderR;
				diffL = ri.rot_encoderL_val - start_encoderL;

				if(diffR - diffL > 1){
					cmd = tec_r_.Calclate(5, 100, Technic::RIGHT);
				}else if(diffL - diffR > 1){
					cmd = tec_r_.Calclate(5, 100, Technic::LEFT);
				}else{
					cmd.param1 = 5;
					cmd.param2 = 0;
					flatten_count++;
				}

				if(flatten_count > 20){
					Robot::Instance().Beep(Robot::B);
					local_status_ = STAIRS_FORWARD2;
					tec_s_.Init(ri);
					TimerStart(4000);
				}

				break;

			// ゆっくり前進
			case STAIRS_FORWARD2:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);

				if(IsTimeup() && evf.is_SetEvent(Robot::EV_GYRO)){
					Robot::Instance().Beep(Robot::A);
					local_status_ = STAIRS_OUT;
					tec_pid_.Init();
					TimerStart(1000);
				}
				break;

			// ライン復帰
			case STAIRS_OUT:
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.2), Technic::RIGHT);

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::C);
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
	unsigned short StairsCounter;// 呼び出し回数記憶エリア
	S32 start_encoderR;
	S32 start_encoderL;
	int flatten_count;
};
