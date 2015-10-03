#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define MAX_SPEED 110
#define S_SPEED 100

namespace 
{
	inline
	S16 calculateMotorPID(S32 encoderL, S32 encoderR, S16 PIDLightParam)
	{
		static S16 prevRotatedDegree = 0; // 前回のPIDによる傾き
		// inclination > 0 : 左に傾いている
		S16 inclination = Navigator::convertToMotorValue( encoderL, encoderR ) - prevRotatedDegree;	// ロボットの傾き[-100,100](ただし前回の光センサPIDによる影響は考えない)
		prevRotatedDegree = -1 * PIDLightParam;	// PID値は制御量なので、制御必要量は-1をかける
		return inclination;
	}
}


class Mission_Slope : public Mission {
private:
	enum LocalState {
		P_START,
		P_STRAIGHT_BEFORE,
		P_UP_SLOPE,
		P_DOWN_SLOPE,
		P_STRAIGHT_AFTER,
	};

	enum {
		DIST_STRAIGHT_BEFORE_OUT	= 1061317,
		DIST_STRAIGHT_BEFORE_IN		= 3287729,
		DIST_UP_SLOPE				= 350000,
		DIST_DOWN_SLOPE				= 1392616,
		SHIFT_LOOKUP				= 3300,
	};

	typedef struct distance{
		float x;
		float y;
		float xy;
	};
	distance dist;


public:

    Mission_Slope(Technic::DIRECTION edge = Technic::LEFT) : Mission(){
		edge_ = edge;
	}

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		timer_start(50);
		speed_ = S_SPEED;
    	local_status_ = P_START;
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    	//Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 10);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		// スタート時はスピードを抑え目にして安定した走りだしをする
		if (P_START == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ ), edge_);
			cmd.param2 += calculateMotorPID( ri.rot_encoderL_val, ri.rot_encoderR_val, cmd.param2 );
			cmd.param3 = 0;
			if (is_timeup()) {
				Robot::Instance().Beep();
				// 状態遷移
				local_status_ = P_STRAIGHT_BEFORE;
				//Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 10);
				//timer_start(5000);
			}
		}

		// 直線走行
		else if (P_STRAIGHT_BEFORE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			cmd.param2 += calculateMotorPID( ri.rot_encoderL_val, ri.rot_encoderR_val, cmd.param2 );
			// サンプルのため、次の状態には遷移しないようにコメントアウトしておきます。

			// //if (is_timeup()) {
			// 	#ifdef IN_COURSE
			// 	if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_STRAIGHT_BEFORE_IN){
			// 	#else
			// 	if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_STRAIGHT_BEFORE_OUT){
			// 	#endif
			// 	Robot::Instance().Beep();
			// 	// 状態遷移
			// 	local_status_ = P_UP_SLOPE;
			// 	dist.x = ni.pos.X;
    	// 		dist.y = ni.pos.Y;
    	// 		dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
			// 	//timer_start(50000);
			// }

		}

		// 坂道ののぼり走行
		else if (P_UP_SLOPE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			//if (is_timeup()) {
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_UP_SLOPE){
				Robot::Instance().Beep();
				// 状態遷移
				local_status_ = P_DOWN_SLOPE;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				//timer_start(5000);
			}
		}

		// 坂道のくだり走行
		else if (P_DOWN_SLOPE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.9), edge_);
			//Robot::Instance().Beep();
			//if (is_timeup()) {
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_DOWN_SLOPE){
				Robot::Instance().Beep();
				// 状態遷移
				local_status_ = P_STRAIGHT_AFTER;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				//timer_start(5000);
			}
		}

    	// 直線走行(arter)
		else if (P_STRAIGHT_AFTER == local_status_) {
			static bool flag = false;
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			if(ri.light_sensor_val < (CourseInfo::colorWhite + 50)){
				if(flag == false){
					timer_start(1000);
					flag = true;
				}
			}else{
				timer_stop();
				speed_ = MAX_SPEED;
				flag = false;
			}
			//if((ni.pos.X > SHIFT_LOOKUP) && Robot::Instance().sonar_distance < 60){ //物体を検知したら次のミッションへ
			if(ni.pos.X > SHIFT_LOOKUP) {
				ret = false;
				Robot::Instance().Beep();
			}
			if (is_timeup()) {
				Robot::Instance().Beep();
				speed_ = S_SPEED;
			}
		}

		else {
			ret = false;
		}

        return ret;

    }



private:

	S16 speed_;

    TecPIDTrace pid_trace_;
	Technic::DIRECTION edge_;

	LocalState local_status_;
};
