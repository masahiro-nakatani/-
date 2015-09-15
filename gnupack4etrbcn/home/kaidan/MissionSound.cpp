#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class MissionSound : public Mission {
private:
private:
	enum LocalState{
		STAIRS_PRE_IN = 0,	 	// �K�i�ՓˑO
		STAIRS_IN,			 	// �K�i�ՓˑO

		STAIRS_HIT_ONE,		// �З֏��グ��
		STAIRS_HIT_BOTH,	// ���֏��グ��
		STAIRS_FLATTEN,		// ���艻
		STAIRS_FORWARD,		// �������O�i

		STAIRS_HIT_ONE2,	// �З֏��グ��
		STAIRS_HIT_BOTH2,	// ���֏��グ��
		STAIRS_FLATTEN2,	// ���艻
		STAIRS_FORWARD2,	// �������O�i

		STAIRS_OUT,			// �K�i���痎��(���C�����A)
		STAIRS_END			// �������s
	};

public:

    MissionSound() : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
		speed_ = 40;
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

			// �������s��(�K�i�ՓˑO)
			// do: PID���C���g���[�X
			// exit: �W���C��
			case STAIRS_PRE_IN:
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(speed_), Technic::RIGHT);

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::A2);
					local_status_ = STAIRS_IN;
					TimerStart(1000);
				}
				break;

			// �������s��(�K�i�ՓˑO)
			// do: PID���C���g���[�X
			// exit: �W���C��
			case STAIRS_IN:
				cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.2), Technic::RIGHT);
				cmd.param3 = 0;

				if(IsTimeup()){
					start_encoderR = ri.rot_encoderR_val;
					start_encoderL = ri.rot_encoderL_val;
					Robot::Instance().Beep(Robot::D);
					local_status_ = STAIRS_HIT_ONE;
					TimerStart(1000);
				}
				break;

			// �З֏��グ��
			// do: ���񑖍s
			// exit: 340ms
			case STAIRS_HIT_ONE:
				cmd = tec_r_.Calclate(30, 100, Technic::LEFT);
				diffR = ri.rot_encoderR_val - start_encoderR;

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::E);
					local_status_ = STAIRS_HIT_BOTH;
					TimerStart(1000);
				}
				break;
			
			// �����З֏��グ��
			// do: ���񑖍s
			// exit: 480ms
			case STAIRS_HIT_BOTH:
				cmd = tec_r_.Calclate(30, 150, Technic::RIGHT);
				diffL = ri.rot_encoderL_val - start_encoderL;

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::F);
					local_status_ = STAIRS_FLATTEN;
					TimerStart(1000);
				}
				break;

			// ���艻��
			// do: ���񑖍s
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

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::B);
					local_status_ = STAIRS_FORWARD;
					TimerStart(1000);
					tec_s_.Init(ri);
				}

				break;

			// �������O�i
			case STAIRS_FORWARD:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);

				if(IsTimeup()){
					start_encoderR = ri.rot_encoderR_val;
					start_encoderL = ri.rot_encoderL_val;
					Robot::Instance().Beep(Robot::C);
					local_status_ = STAIRS_HIT_ONE2;
					TimerStart(1000);
				}
				break;
			

			// �З֏��グ��
			// do: ���񑖍s
			case STAIRS_HIT_ONE2:
				cmd = tec_r_.Calclate(30, 100, Technic::RIGHT);
				diffL = ri.rot_encoderL_val - start_encoderL;


				if(IsTimeup()){
					Robot::Instance().Beep(Robot::E);
					local_status_ = STAIRS_HIT_BOTH2;
					TimerStart(1000);
				}
				break;
			
			// �����З֏��グ��
			// do: ���񑖍s
			case STAIRS_HIT_BOTH2:
				cmd = tec_r_.Calclate(30, 150, Technic::LEFT);
				diffR = ri.rot_encoderR_val - start_encoderR;

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::F);
					local_status_ = STAIRS_FLATTEN2;
					flatten_count = 0;
					TimerStart(1000);
				}
				break;

			// ���艻��
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

				if(IsTimeup()){
					Robot::Instance().Beep(Robot::B);
					local_status_ = STAIRS_FORWARD2;
					tec_s_.Init(ri);
					TimerStart(1000);
				}

				break;

			// �������O�i
			case STAIRS_FORWARD2:
				cmd = tec_s_.Calclate(10, ri.rot_encoderL_val, ri.rot_encoderR_val);
				
				if(IsTimeup()){
					Robot::Instance().Beep(Robot::A);
					local_status_ = STAIRS_OUT;
					tec_pid_.Init();
					TimerStart(1000);
				}
				break;

			// ���C�����A
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
	unsigned short StairsCounter;// �Ăяo���񐔋L���G���A
	S32 start_encoderR;
	S32 start_encoderL;
	int flatten_count;

//    static Mission_Default instance_;

};
