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
		P_READY_TO_TOUCH1_1,
		P_READY_TO_TOUCH1_2,
		P_MIDDLE_OF_SEESAW2,
		P_READY_TO_TOUCH2_1,
		P_READY_TO_TOUCH2_2,
		P_MIDDLE_OF_SEESAW3,
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
        speed_ = 125;
		local_status_ = P_CLIMBING;
        s_time_ = clock.now();
		tecType_ = TEC_P;
        ni_start.pos = ni.pos;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
        F32 r;
        r = sqrt(pow(ni.pos.X - ni_start.pos.X, 2) + pow(ni.pos.Y - ni_start.pos.Y, 2));

        if ( local_status_ == P_CLIMBING){
//			if((ni.pos.X > 1650 && ni.pos.Y > 2000) && evf.is_SetEvent(Robot::EV_GYRO)){
			if( evf.is_SetEvent(Robot::EV_GYRO)){
				Robot::Instance().Beep(Robot::C);   //ど
				//ステート変更
				local_status_ = P_MIDDLE_OF_SEESAW;
				speed_ = 50;
                tecType_ = TEC_P;
                ni_start.pos = ni.pos;
				timer_start(1500);
                //バッテリー　時間
                // 82           1200
                // 80           1400x 2000o
			}
        }
		if ( local_status_ == P_MIDDLE_OF_SEESAW){
            if (is_timeup()){
				local_status_ = P_READY_TO_TOUCH1_1;
				Robot::Instance().Beep(Robot::D);   //れ
                speed_ = 10;
                tecType_ = TEC_P;
            }
        }
		if ( local_status_ == P_READY_TO_TOUCH1_1){
            if (r > 320){
                //500x>
                //300x>
                //250x>
                //245?<
                //240?<
                //235x<
                //225x<
                //200x<
			    //350x> //batt83
			    //300x< //batt81
			    //320?  //batt81
			    local_status_ = P_READY_TO_TOUCH1_2;
				Robot::Instance().Beep(Robot::E);   //み
                speed_ = -50;
				timer_start(3000);
            }
        }
		if ( local_status_ == P_READY_TO_TOUCH1_2){
             if (is_timeup() || evf.is_SetEvent(Robot::EV_GYRO3)){
				local_status_ = P_MIDDLE_OF_SEESAW2;
				Robot::Instance().Beep(Robot::F);   //ふぁ
				speed_ = -150;
                tecType_ = TEC_S;
				timer_start(1000);
            }
		}
        if ( local_status_ == P_MIDDLE_OF_SEESAW2){
            if (is_timeup()){
				Robot::Instance().Beep(Robot::E);   //み	
                speed_ = -100;
                tecType_ = TEC_S;
//				timer_start(3300);
				local_status_ = P_READY_TO_TOUCH2_1;
            }
        }

		if ( local_status_ == P_READY_TO_TOUCH2_1){
            if (r < 240){
                //500x>
                //300x>
                //250x>
                //245?<
                //240?<
                //235x<
                //225x<
                //200x<
				local_status_ = P_READY_TO_TOUCH2_2;
				Robot::Instance().Beep(Robot::D);   //れ
                speed_ = 0;
                tecType_ = TEC_S;
				timer_start(1000);
            }
        }
		if ( local_status_ == P_READY_TO_TOUCH2_2){
             if (is_timeup() || evf.is_SetEvent(Robot::EV_GYRO3)){
				local_status_ = P_MIDDLE_OF_SEESAW3;
				Robot::Instance().Beep(Robot::C);   //ど
				speed_ = 50;
                tecType_ = TEC_S;
				timer_start(1000);
            }
		}
		if ( local_status_ == P_MIDDLE_OF_SEESAW3){
			if(is_timeup()){
				local_status_ = P_DOWN;
				Robot::Instance().Beep(Robot::D);   //れ
				speed_ = 10;
				timer_start(3300);
			}
		}
        if ( local_status_ == P_DOWN){
			if (is_timeup() || evf.is_SetEvent(Robot::EV_GYRO)){
				Robot::Instance().Beep(Robot::E);   //み
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
				Robot::Instance().Beep(Robot::F);   //ふぁ
			}
		}


		if(local_status_ == P32){
			if(is_timeup()){
				tecType_ = TEC_P;
				speed_ = 80;
				local_status_ = P35;
			}
		}

		if( local_status_ == P35){
			if(ni.pos.X > 3100){
				//終了
				Robot::Instance().Beep(Robot::G);   //そ
				speed_ = 125;
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
        
        cmd.param3 = 40;

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

    NavInfo ni_start;
//    static Mission_Seesaw instance_;

    LocalState local_status_;

};


//Mission_Seesaw Mission_Seesaw::instance_;
