#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


namespace
{
	static const Vec POINTS[1] ={
		Vec(1,1)
	};

	static const int DIST_S_TURN_POINT_IN_1 = 100000;
	static const int DIST_S_TURN_POINT_IN_2 = 100000;
	static const int DIST_S_TURN_POINT_IN_3 = 100000;
	static const int DIST_S_TURN_POINT_IN_4 = 100000;
	static const int DIST_S_TURN_POINT_IN_5 = 100000;
	static const int DIST_S_TURN_POINT_IN_6 = 100000;
	static const int DIST_S_TURN_POINT_IN_7 = 100000;

}

//---------------------------------------------------------------------------
/*!
 * @brief   S字攻略のためのミッション
 * @see     
 * 
 *   基本的にMission_Defaltをベースに作成している
 */
//---------------------------------------------------------------------------
class Mission_S_Turn : public Mission {

	typedef List<Vec> PointList;
	static bool check_distance(const Vec& lhs, const Vec& rhs)
	{
		return (lhs.sub(rhs).length()) < 5.0;
	}


public:

    Mission_S_Turn(S32 timer = 1000, S16 speed = 80, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
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

		if(step_ == 0){			/* S字ターン開始位置までライントレース */
			cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, Technic::LEFT);
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                Robot::Instance().Beep();
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 1){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            /* 終了条件 */
            if(IsTimeup()){
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//		|
        else if(step_ == 2){    /* 左旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = -50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
                dist.x = ni.pos.X;
                dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 3){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_2){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 4){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//   ___
		//		|
        else if(step_ == 5){    /* 右旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 6){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_3){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 7){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//  |___
		//		|
        else if(step_ == 8){    /* 右旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
		}
        else if(step_ == 9){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_4){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 10){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//	_________
		//  |___
		//		|
        else if(step_ == 11){    /* 左旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = -50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
                dist.x = ni.pos.X;
                dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 12){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_5){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 13){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//	_________|
		//  |___
		//		|
        else if(step_ == 14){    /* 左旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = -50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
                dist.x = ni.pos.X;
                dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 15){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_6){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 16){    /* 一時停止 */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* 終了条件 */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//       ____
		//	_________|
		//  |___
		//		|
        else if(step_ == 17){    /* 右旋回(90度) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* 終了条件 */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
                step_++;
            }
		}
        else if(step_ == 18){    /* 直進 */
            cmd.param1 = 50;
            cmd.param2 = 0;
            /* 終了条件 */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_7){
                TimerStart(100);
                step_++;
            }
        }

		cmd.param3 = 0;

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
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_Default instance_;

};
