#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"
#include <stdlib.h>


namespace
{
	static const Vec POINTS[1] ={
		Vec(1,1)
	};

	static const int DIST_S_TURN_POINT_IN_1 = 1000000;
	static const int DIRECT_OFFSET = 8;
	static const int DIRECT_OFFSET_TURN = 4;
	static const int TAIL_ANG = 90;
	static const int BALLON_SPEED = 50;

}

//---------------------------------------------------------------------------
/*!
 * @brief   S字攻略のためのミッション
 * @see     
 * 
 *   基本的にMission_Defaltをベースに作成している
 */
//---------------------------------------------------------------------------
class MissionBalloon : public Mission {

	typedef List<Vec> PointList;
	static bool check_distance(const Vec& lhs, const Vec& rhs)
	{
		return (lhs.sub(rhs).length()) < 5.0;
	}


public:

    MissionBalloon(S32 timer = 1000, S16 speed = 50, S16 tail = 90) : Mission() {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail; 
		dist.x = 0;
        dist.y = 0;
        step_ = 0;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		if (timer_ > 0) {
			TimerStart(timer_);
		}
        Robot::Instance().Beep();
		//Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);

        dist.x = ni.pos.X;
        dist.y = ni.pos.Y;
		step_ = 0;
		speed_=80;
		//Navigator::Instance().StartDirCorrection (400.0, 180.0);
    }


    //---------------------------------------------------------------------------
	/*!
	 * @brief   Mission実行
	 * @param[in] ri ロボットの最新情報. エンコーダ値などが記録されている
	 * @param[in] ni 座標情報. ロボット情報が座標値に変換されたデータが入っている
	 * @param[in] evf イベントフラグ
	 * @param[out] cmd ロボットの出力情報. 出力値は速度と方向
	 * @returns true : Mission続行, false : Mission終了
	 * 
	 *   ここに Run の詳細を記載.
	 */
	//---------------------------------------------------------------------------
	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		Vec now( ni.pos.X, ni.pos.Y );	// 現在位置(x,y)mm


		// F32 f = now.dot( prevVec_ ); // たとえば、これはベクトルの内積を計算し、floatで保存


		// cmd.param1 = speed_; スピードを入れる NORMAL_MODEならSpeed(%)[-100, 100], DIRECT_MODEなら左モータのPWM値[-100, 100]
		// cmd.param2 = direction; 向きを入れる NORMAL_MODEなら向き(%)[-100, 100], DIRECT_MODEなら右モータの[PWM][-100, 100]
		// このURLを参照ください：http://lejos-osek.sourceforge.net/ecrobot_c_api_frame_jp.htm

		// 最後に前回位置として保存

		prevVec_ = now;

		/////以下、既存のコード///////
		//(不必要なら消してください)

       // cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, Technic::LEFT);
		
		/*if (IsTimeup()) {
			ret = false;
			Robot::Instance().Beep();
		}*/

		/*尻尾を下す*/
		cmd.param3 = TAIL_ANG;

		if(step_ == 0){
			Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 86);
			step_++;
		}

		else if(step_ == 1){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 2){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//		|
		//		|
        else if(step_ == 3){    /* 右旋回(90度) */
            cmd.param1 = 70;
            cmd.param2 = -70+DIRECT_OFFSET_TURN;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 4){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 5){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |
		//		|
        else if(step_ == 6){    /* 右旋回(90度) */
            cmd.param1 = 70;
            cmd.param2 = -70+DIRECT_OFFSET_TURN;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
		}
        else if(step_ == 7){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 8){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |
		//		|
        else if(step_ == 9){    /* 右旋回(90度) */
            cmd.param1 = 70;
            cmd.param2 = -70+DIRECT_OFFSET_TURN;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 10){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 11){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |     |
		//		|     |
        else if(step_ == 12){    /* 右旋回(90度) */
            cmd.param1 = 70;
            cmd.param2 = -70+DIRECT_OFFSET_TURN;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 13){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1/8){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 14){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |     |
		//		|   __|
        else if(step_ == 15){    /* 右旋回(90度) */
            cmd.param1 = 70;
            cmd.param2 = -70+DIRECT_OFFSET_TURN;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
                step_++;
            }
		}
        else if(step_ == 16){    /* 直進 */
            cmd.param1 = 70;
            cmd.param2 = 70+DIRECT_OFFSET;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1/5){
                TimerStart(100);
                step_++;
				turn = 0;
            }
        }

		//step_ = 17;
		else if(step_ == 17){			/* 暴走！！ */

			if(turn == 0)	bousou = rand()%1000;

			if(bousou <= 990){
				cmd.param1 = 70;
				cmd.param2 = 70+DIRECT_OFFSET;
				TimerStart(100);
			}
			else if(bousou > 990 && bousou < 997){	/*バック*/
				if(turn == 0){
					turn_ang = rand()%100;
					TimerStart(100);
					cmd.param1 = 0;
					cmd.param2 = 0;
					base_ang_ = ni.pos.Ang;

					turn = 1;
				}
				else if(turn == 1){
					TimerStart(1000);
					cmd.param1 = -30;
					cmd.param2 = -30+DIRECT_OFFSET;

					/* 終了条件 */
					if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) < DIST_S_TURN_POINT_IN_1/3){
						TimerStart(100);
						turn = 2;
					  }
				}
				else if(turn == 2){
					TimerStart(100);
					cmd.param1 = -70;
					cmd.param2 = 70+DIRECT_OFFSET;
					/* 終了条件 */
					if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
						turn = 0;
					}
				}

			}
			else if(bousou == 997){	/*左旋回*/
				if(turn == 0){
					turn_ang = rand()%100;
					TimerStart(100);
					cmd.param1 = 0;
					cmd.param2 = 0;
					base_ang_ = ni.pos.Ang;
				}
				turn = 1;

				TimerStart(100);
				cmd.param1 = -70;
				cmd.param2 = 70+DIRECT_OFFSET_TURN;

				/* 終了条件 */
				if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
					turn = 0;
				}
			}

			else if(bousou == 998){	/*右旋回*/
				if(turn == 0){
					turn_ang = rand()%100;
					TimerStart(100);
					cmd.param1 = 0;
					cmd.param2 = 0;
					base_ang_ = ni.pos.Ang;
				}
				turn = 1;

				TimerStart(100);
				cmd.param1 = 70;
				cmd.param2 = -70+DIRECT_OFFSET_TURN;

				/* 終了条件 */
				if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
					turn = 0;
				}

			}

			else if(bousou == 999){	/* 一時停止 */
                TimerStart(100);
				cmd.param1 = 0;
				cmd.param2 = 0;

			}
			else if(bousou == 1000){	/* 超暴走 */
				cmd.param1 = 100;
				cmd.param2 = 100;
                TimerStart(3000);
			}
				

        }

		
		cmd.Mode = RobotCmd::DIRECT_MODE;
        return ret;

    }



private:

    TecPIDTrace pid_trace_;
	Vec prevVec_;//< 前回の位置(x,y)mm

	typedef struct distance{
		float x;
		float y;
		float xy;
	};
	distance dist;
	F64 base_ang_;
	S16 step_;
	S16 bousou;
	S16 turn;
	S16 turn_ang;
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_Default instance_;

};
