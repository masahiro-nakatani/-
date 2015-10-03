#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

class Mission_Seesaw : public Mission {
private:
    enum LocalState{
        P0 = 0,
		P_CLIMBING,
		P_MIDDLE_OF_SEESAW,

        P_DOWN,
		P_RECOVER,
		P32,
		P35,
    };

	enum TecType{
		TEC_S,
		TEC_P,
		TEC_R,
		TEC_O,
	};

public:

    Mission_Seesaw(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        tec_s_.Init(ri);
		tec_p_.Init();
		tec_r_.Init();
//		speed_ = 50;
        speed_ = 70;
		local_status_ = P_CLIMBING;
        s_time_ = clock.now();
		tecType_ = TEC_P;
    	SeesawAng = ni.pos.Ang;
    	timer_start(15000);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

      //  if ( local_status_ == P_CLIMBING){
        //	if(is_timeup() && ((SeesawAng - ni.pos.Ang) < (5.0/180 * M_PI) && (SeesawAng - ni.pos.Ang) > (-5.0/180 *M_PI))){
        //		speed_ = 125;
        //		Robot::Instance().Beep();
        //	}
//			if((ni.pos.X > 1650 && ni.pos.Y > 2000) && evf.is_SetEvent(Robot::EV_GYRO)){
		//	if( speed_ == 125 && evf.is_SetEvent(Robot::EV_GYRO)){
		if( evf.is_SetEvent(Robot::EV_GYRO)){
			Robot::Instance().Beep();
				//ステート変更
				local_status_ = P_MIDDLE_OF_SEESAW;
//				speed_ = 80;
				speed_ = 50;
				timer_start(1200);
		//	}
        }
		if ( local_status_ == P_MIDDLE_OF_SEESAW){
			speed_ = 50;
			if(is_timeup()){
				local_status_ = P_DOWN;
				Robot::Instance().Beep(Robot::B);
				speed_ = 10;
				timer_start(3300);
			}
		}

        if ( local_status_ == P_DOWN){
			if (is_timeup() || evf.is_SetEvent(Robot::EV_GYRO)){
				Robot::Instance().Beep();
				speed_ = 20;
				local_status_ = P_RECOVER;
				tecType_ = TEC_S;
				tec_s_.Init(ri);
			}
        }

		if(local_status_ == P_RECOVER){
			if(evf.is_SetEvent(Robot::EV_LIGHT_BLACK)){
				local_status_ = P32;
				speed_ = 0;
				rotate_ = 10;
				dir_ = Technic::RIGHT;
				tecType_ =TEC_R;
				timer_start(700);
				Robot::Instance().Beep(Robot::B);
			}
		}


		if(local_status_ == P32){
			if(is_timeup()){
				tecType_ = TEC_P;
				speed_ = 80;
				timer_start(1000);
				local_status_ = P35;
			}
		}

		if( local_status_ == P35){
			if(is_timeup()){
				//終了
				Robot::Instance().Beep(Robot::E);
				speed_ = 80;
				ret = false;
			}
		}



		

		//走行計算
		if(tecType_ == TEC_S){
			cmd = tec_s_.Calclate(speed_,
                              ri.rot_encoderL_val,
                              ri.rot_encoderR_val
                              );
			if(local_status_ == P_RECOVER){
				cmd.param2 =- 5; // 回転力ちょっと弱め
			}
		}else if(tecType_ == TEC_P){
			cmd = tec_p_.Calclate(ri.light_sensor_val, speed_, Technic::RIGHT);
			if(local_status_ == P_MIDDLE_OF_SEESAW || local_status_ == P_DOWN){
				cmd.param2 = (S16)((F32)cmd.param2 * 0.7);
			}
		}else if(tecType_ == TEC_R){
			cmd = tec_r_.Calclate(speed_, rotate_, dir_);
		}else{
		}
        
        //cmd.param3 = 40;
		cmd.param3 = 0;

        return ret;
    }


    virtual char* Name(void){
        return "Mission_Seesaw";
    }

private:

    S16 speed_;
	S16 rotate_;
	Technic::DIRECTION dir_;
    S32 s_time_;

    bool is_start;
	short tecType_;
    TecStraight tec_s_;
	TecPIDTrace tec_p_;
	TecRotate tec_r_;
    float SeesawAng;

//    static Mission_Seesaw instance_;

    LocalState local_status_;

};


//Mission_Seesaw Mission_Seesaw::instance_;
