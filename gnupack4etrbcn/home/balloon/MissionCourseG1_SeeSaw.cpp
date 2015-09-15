#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

class Mission_CourseG1_SeeSaw : public Mission {
private:
    enum LocalState{
		P_REACHING,
        S_APPROACH_BACK,
		S_STAGING,
        P_RECOVER_ON_SEESAW,
		S_GET_OFF,
		S_RECOVER_ON_GROUND,
		P_FOR_NEXT_MISSION,
    };

	enum TecType {
		TEC_S,
		TEC_P,
	};

	typedef struct position {
		float x;
		float y;
	} Position;
   

    void set_current_position(Position& pos, const NavInfo& ni) {
        pos.x = ni.pos.X;
    	pos.y = ni.pos.Y;
    }

    float get_distance(const Position& pos, const NavInfo& ni) const {
        const float dx = (ni.pos.X - pos.x);
        const float dy = (ni.pos.Y - pos.y);
        return dx*dx + dy*dy;        
    }


public:
    Mission_CourseG1_SeeSaw(S32 timer, S16 speed, S16 tail, Technic::DIRECTION edge)
		: Mission()
	{
		timer_ = timer;
		speed_ = speed;
		tail_ = tail;
		edge_ = edge;
    }

    virtual void Init(RobotInfo ri, NavInfo ni)
	{
        tec_s_.Init(ri);
		tec_p_.Init();
		tecType_ = TEC_P;

        pid_trace_.Init();
		if (0 < timer_) {
			TimerStart(timer_);
		}
		local_status_ = P_REACHING;

    	SeesawAng = ni.pos.Ang;
        set_current_position(pos, ni);
    }


	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd)
	{
		bool ret = true;

		switch (local_status_) {
		case P_REACHING:		// シーソーに近づく.
			speed_ = 10;
			// 終了条件：ジャイロの変化があったら終了.
            if (evf.is_SetEvent(Robot::EVENT_GYRO)) {
                Robot::Instance().Beep(Robot::C);
                evf.Set(Robot::EVENT_GYRO, false);
                set_current_position(pos, ni);
				local_status_ = S_APPROACH_BACK;

                // 助走するために少し下がる.
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = -30;
            }
            break;

        case S_APPROACH_BACK:	// 助走するために少し下がる.
			// 終了条件：一定距離進んだら終了.
            if (100 * 100 < get_distance(pos, ni)) {
                Robot::Instance().Beep(Robot::D);
                set_current_position(pos, ni);
                local_status_ = S_STAGING;

                // 一気にシーソーへ登る.
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = 100;
            }
            break;

        case S_STAGING:			// 一気にシーソーへ登る.
            if (180 * 180 < get_distance(pos, ni)) {
                Robot::Instance().Beep(Robot::E);
                set_current_position(pos, ni);
                local_status_ = P_RECOVER_ON_SEESAW;
                    
                // とりあえずシーソー上でラインに復帰.
                tecType_ = TEC_P;
                tec_p_.Init();
                speed_ = 10;
            }
            break;

        case P_RECOVER_ON_SEESAW: // とりあえずシーソー上でラインに復帰.
			// 終了条件：一定距離進んだら終了.
            if (200 * 200 < get_distance(pos, ni)) {
                Robot::Instance().Beep(Robot::F);
                set_current_position(pos, ni);
                local_status_ = S_GET_OFF;

				// 左にそれながらシーソーを降りる.
				tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = 40;
            }
            break;

        case S_GET_OFF:		// 左にそれながらシーソーを降りる.
			// 終了条件：一定距離進んだら終了.
            if (250 * 250 < get_distance(pos, ni)) {
                Robot::Instance().Beep(Robot::G);
                local_status_ = S_RECOVER_ON_GROUND;

                // ラインに復帰を試みる
                tec_s_.Init(ri);
                tecType_ = TEC_S;
                speed_ = 10;

            }
            break;

        case S_RECOVER_ON_GROUND:
			// 終了条件：ライン検知したら終了.
            if (evf.is_SetEvent(Robot::EVENT_LIGHT_BLACK)){
                Robot::Instance().Beep(Robot::A);
                set_current_position(pos, ni);
                local_status_ = P_FOR_NEXT_MISSION;

				// ライントレースして完全復帰する.
				tecType_ = TEC_P;
                tec_p_.Init();
                speed_ = 20;
                TimerStart(10000);
            }
            break;

        case P_FOR_NEXT_MISSION: // 最後に安定するまでライントレースして完全復帰する
			// 終了条件：一定距離進んだら終了.
            if (200 * 200 < get_distance(pos, ni)) {
                Robot::Instance().Beep(Robot::A2);
                ret = false;                 
            }
            break;
        } // end of switch-case(local_status_)


		// 走行計算.
		switch (tecType_) {
		case TEC_S:
			cmd = tec_s_.Calclate(
				speed_,
				ri.rot_encoderL_val,
                ri.rot_encoderR_val
				);
			switch (local_status_) {
			case S_GET_OFF:
				cmd.param2 =- 3; // 左側へ降りる
				break;
			case S_RECOVER_ON_GROUND:
				cmd.param2 =+ 3; // 右側のラインへ復帰する
				break;
			default:
				// 何もなし
				break;
			}
			break;

		case TEC_P:
			cmd = tec_p_.Calclate(ri.light_sensor_val, speed_, edge_);
			break;

		default:
			break;
		}

		return ret;
    }


    virtual char* Name(void){
        return "Mission_CourseG1_SeeSaw";
    }



private:
	S32 timer_;
	S16 speed_;
	S16 tail_;
	Technic::DIRECTION edge_;

	TecPIDTrace pid_trace_;
	LocalState local_status_;

	short tecType_;
    TecStraight tec_s_;
	TecPIDTrace tec_p_;
    float SeesawAng;

	Position pos;
};


//Mission_Seesaw Mission_Seesaw::instance_;












