#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define MAX_SPEED 110
#define S_SPEED 110


class Mission_Slope : public Mission {
private:
	enum LocalState {
		P_START,
		P_STRAIGHT_BEFORE,
		P_UP_SLOPE,
		P_DOWN_SLOPE,
		P_SHORTCUT,
		P_STRAIGHT_AFTER,
	};

	enum {
#ifdef IN_COURSE
		DIST_STRAIGHT_BEFORE		= (1814 * 1814),	// スタート(IN)位置から坂道開始までの距離
		DIST_GOAL_GATE				= (1180 * 1180),	// スタート(IN)位置からゴールゲート(IN)までの距離
		SLOPE_END_X					= 1585,				// 坂道終了X座標
		SLOPE_END_Y					= 506,				// 坂道終了Y座標
#else
		DIST_STRAIGHT_BEFORE		= (1020 * 1020),	// スタート(OUT)位置から坂道開始までの距離
		DIST_GOAL_GATE				= (1100 * 1100),	// スタート(OUT)位置からゴールゲート(OUT)までの距離
		SLOPE_END_X					= 1585,				// 坂道終了X座標
		SLOPE_END_Y					= 256,				// 坂道終了Y座標
#endif
		DIST_UP_SLOPE				= (566 * 566),		// 坂道開始から坂道頂点までの距離
		DIST_DOWN_SLOPE				= (1142 * 1142),	// 坂道頂点から坂道終了までの距離
		DIST_SHORTCUT_POINT_IN		= (1500 * 1500),	// 坂道終了からショートカット開始位置までの距離
		DIST_SHORTCUT_FORWARD		= (210 * 210),		// ショートカットで進む距離
		SHIFT_LOOKUP_X				= 3600,
		SHIFT_LOOKUP_Y				= 2000,
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
		TimerStart(50);
		speed_ = S_SPEED;
    	local_status_ = P_START;
    	CourseInfo::getStartPos(startPos.X, startPos.Y, startPos.Ang);
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool stay_mission = true;
        
		switch(local_status_){
            
            // スタート時はスピードを抑え目にして安定した走りだしをする
            case P_START:
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ ), edge_);
                cmd.param3 = 0;
                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = P_STRAIGHT_BEFORE;
                }
                break;

            // 直線走行
            case P_STRAIGHT_BEFORE:
            
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
                bool exit_condition = ((ni.pos.X - startPos.X) * (ni.pos.X - startPos.X) +( ni.pos.Y - startPos.Y) * ( ni.pos.Y - startPos.Y)) > DIST_STRAIGHT_BEFORE;
                
                /* 終了条件 */
                if(exit_condition){
                    Robot::Instance().Beep();
                    local_status_ = P_UP_SLOPE;
                    
                    /* 初期化 */
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                }
                break;

            // 坂道ののぼり走行
            case P_UP_SLOPE:
            
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
                
                /* 終了条件 */
                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_UP_SLOPE){
                    Robot::Instance().Beep();
                    local_status_ = P_DOWN_SLOPE;

                    /* 初期化 */
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                }
                break;

            // 坂道のくだり走行
            case P_DOWN_SLOPE:
                
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.9), edge_);
                
                /* 終了条件*/
                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_DOWN_SLOPE){
                    Robot::Instance().Beep();
#ifdef IN_COURSE
                    step_ = 0;
                    local_status_ = P_SHORTCUT;
//                  local_status_ = P_STRAIGHT_AFTER;		// ショートカットしない場合
#else
                    TimerStart(49000);
                    local_status_ = P_STRAIGHT_AFTER;
#endif
                    
                    Navigator::Instance().SetInfo(SLOPE_END_X, SLOPE_END_Y, 3.14159);
                    /* 初期化 */
                    dist.x = SLOPE_END_X;
                    dist.y = SLOPE_END_Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                }
                break;

            case P_SHORTCUT:		/* ショートカット(INコースのみ) */
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);

                if(step_ == 0){			/* ショートカット開始位置までライントレース */
                    /* 終了条件 */
                    if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_SHORTCUT_POINT_IN){
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
                    if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_SHORTCUT_FORWARD){
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
                else if(step_ == 5){    /* 左旋回(90度) */
                    cmd.param1 = 0;
                    cmd.param2 = -50;
                    /* 終了条件 */
                    if((ni.pos.Ang - base_ang_) >= (90.0F * M_PI) / 180.0F){
                        step_++;
                    }
                }
                else if(step_ == 6){    /* レーン復帰 */
                    cmd.param1 = 20;
                    cmd.param2 = -3;
                    /* 終了条件 */
                    if(evf.is_SetEvent(Robot::EVENT_LIGHT_BLACK)){
                        Robot::Instance().Beep();
                        TimerStart(500);
                        speed_ = 0;
                        step_++;
                    }
                }
                else if(step_ == 7){    /* レーン復帰(右回転) */
                    cmd.param1 = speed_;
                    cmd.param2 = 50;
                    /* 終了条件 */
                    if(IsTimeup()){
                        pid_trace_.Init();
                        TimerStart(200);
                        step_++;
                    }
                }
                else if(step_ == 8){    /* スピードを徐々に戻す */
                    if(IsTimeup()){
                        speed_ += 10;
                        TimerStart(200);
                    }
                    /* 終了条件 */
                    if(speed_ >= S_SPEED){
                        Robot::Instance().Beep();
                        local_status_ = P_STRAIGHT_AFTER;
                        speed_ = S_SPEED;
                        step_++;

                        /* 初期化 */
                        dist.x = ni.pos.X;
                        dist.y = ni.pos.Y;
                        dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    }
                }
                break;

            // 直線走行(arter)
            case P_STRAIGHT_AFTER:
                
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
                
                if (IsTimeup()) {
//                  Robot::Instance().Beep();
                    speed_ = 100;
                }
                
                /* 終了条件 */
                if(ni.pos.X > SHIFT_LOOKUP_X && ni.pos.Y < SHIFT_LOOKUP_Y) {
                    stay_mission = false;
                    Robot::Instance().Beep();
                }

                break;

            default:
                stay_mission = false;
                break;
		}

        return stay_mission;

    }



private:

	S16 speed_;
	S16 step_;
	F64 base_ang_;
	Position startPos;

    TecPIDTrace pid_trace_;
	Technic::DIRECTION edge_;

	LocalState local_status_;
};


