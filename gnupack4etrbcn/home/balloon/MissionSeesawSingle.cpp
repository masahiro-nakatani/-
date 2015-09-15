#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"

class Mission_Seesaw : public Mission {
private:
    enum LocalState{
        P0 = 0,
		P_REACHING,
        S_APPROACH_BACK,
		S_STAGING,
        P_RECOVER_ON_SEESAW,
		S_GET_OFF,
		S_RECOVER_ON_GROUND,
		P_FOR_NEXT_MISSION,
    };

	enum TecType{
		TEC_S,
		TEC_P,
		TEC_R,
		TEC_O,
        TEC_T, // しっぽを下して走行する
	};

	typedef struct position{
		float x;
		float y;
	} Position;
   

    void set_current_position(Position& pos, const NavInfo& ni){
        pos.x = ni.pos.X;
    	pos.y = ni.pos.Y;
    }

    float get_distance(const Position& pos, const NavInfo& ni) const {
        const float dx = (ni.pos.X - pos.x);
        const float dy = (ni.pos.Y - pos.y);
        return dx*dx + dy*dy;        
    }

public:

    Mission_Seesaw(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        tec_s_.Init(ri);
		tec_p_.Init();
		tec_r_.Init();
        speed_ = 40;
		local_status_ = P_REACHING;
        s_time_ = clock.now();
		tecType_ = TEC_P;
    	SeesawAng = ni.pos.Ang;
    	TimerStart(15000);
        set_current_position(pos, ni);
        edge_dir = Technic::LEFT;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool stay_mission = true;
		
		switch (local_status_){
        case P_REACHING: //シーソーに近づく
            speed_ = 10;
            if( evf.is_SetEvent(Robot::EVENT_GYRO)){
                evf.Set(Robot::EVENT_GYRO, false);
                Robot::Instance().Beep();
                set_current_position(pos, ni);
				local_status_ = S_APPROACH_BACK;

                // 助走するために少し下がる
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = -30;
            }
            break;

        case S_APPROACH_BACK:
            if( get_distance(pos, ni) > 100*100){
                set_current_position(pos, ni);
                Robot::Instance().Beep();
                local_status_ = S_STAGING;

                // 一気に登る
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = 100;
            }
            break;

        case S_STAGING: // 一気にシーソーへ登る
            if( get_distance(pos, ni) > 180*180){
                set_current_position(pos, ni);
                Robot::Instance().Beep(Robot::B);
                local_status_ = P_RECOVER_ON_SEESAW;
                    
                // とりあえずシーソー上でラインに復帰
                tecType_ = TEC_P;
                tec_p_.Init();
                speed_ = 10;
            }
            break;

        case P_RECOVER_ON_SEESAW: // とりあえずシーソー上でラインに復帰
            if( get_distance(pos, ni) > 200*200 ){
                set_current_position(pos, ni);
                Robot::Instance().Beep();
                local_status_ = S_GET_OFF; //左にそれながらシーソーを降りる
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = 40;
            }
            break;

            case S_GET_OFF:
                if(get_distance(pos, ni) > 250*250){
                    Robot::Instance().Beep(Robot::B);
                    local_status_ = S_RECOVER_ON_GROUND;

                    // ラインに復帰を試みる
                    tec_s_.Init(ri);
                    tecType_ = TEC_S;
                    speed_ = 10;

                }
                break;

            case S_RECOVER_ON_GROUND:
                /* 終了条件　*/
                if(evf.is_SetEvent(Robot::EVENT_LIGHT_BLACK)){
                    set_current_position(pos, ni);
                    local_status_ = P_FOR_NEXT_MISSION;
                    Robot::Instance().Beep(Robot::G);
                    tecType_ = TEC_P;
                    tec_p_.Init();
                    speed_ = 20;
                    TimerStart(10000);
                }
                break;

            case P_FOR_NEXT_MISSION: // 最後に安定するまでライントレースして完全復帰する
                /* ミッション終了の条件 */
                if(get_distance(pos, ni) > 200*200){
                    Robot::Instance().Beep(Robot::E);
                    stay_mission = false;                 
                }
                break;
                
            default:
                stay_mission = true;
                break;
        } // end of switch-case(local_status_)


		/* 走行計算 */
		if(tecType_ == TEC_S){
			cmd = tec_s_.Calclate(speed_,
                              ri.rot_encoderL_val,
                              ri.rot_encoderR_val
                              );
			if(local_status_ == S_GET_OFF){
				cmd.param2 =- 3; // 左側へ降りる
			}
			if(local_status_ == S_RECOVER_ON_GROUND){
				cmd.param2 =+ 3; // 右側のラインへ復帰する
			}

		}else if(tecType_ == TEC_P){
			cmd = tec_p_.Calclate(ri.light_sensor_val, speed_, edge_dir);
			// if(local_status_ == S_STAGING || local_status_ == P_RECOVER_ON_SEESAW){
			// 	cmd.param2 = (S16)((F32)cmd.param2 * 0.7);
			// }
		}else if(tecType_ == TEC_R){
			cmd = tec_r_.Calclate(speed_, rotate_, dir_);
		}else if(tecType_ == TEC_T){
            // しっぽの設定はcase内で設定するよ
		}else{}
        
        return stay_mission;
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
    Technic::DIRECTION edge_dir;

	Position pos;
    

//    static Mission_Seesaw instance_;

    LocalState local_status_;

};


//Mission_Seesaw Mission_Seesaw::instance_;












