#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define LOOKUPENCODER 38



class Mission_LookupGate : public Mission {
private:
	enum LocalState {
		P_START,
		P_STRAIGHT_BEFORE,
		P_STOP,
		P_LOOKUP,
		P_STOP_LOOKUP,
		P_STOP_LOOKUP2,
		P_STRAIGHT_LOOKUP,
		P_BACK,
		P_STANDUP,//入らない子
		P_STRAIGHT,
		P_STRAIGHT2
	};

	enum LocalDifinitions {
		DIST_STRAIGHT_BEFORE_OUT = 100000,
		DIST_STRAIGHT_BEFORE_IN  = 3287729,
		DIST_UP_SLOPE			 = 350000,
		DIST_DOWN_SLOPE			 = 1392616,
		DIST_LOOKUP				 = 100000,
	};

	typedef struct distance{
		float x;
		float y;
		float xy;
	};
	distance dist;


public:

    Mission_LookupGate(Technic::DIRECTION edge = Technic::RIGHT) : Mission(){
		edge_ = edge;
	}

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
    	straight_.Init(ri);
		TimerStart(1000);
		speed_ = 70;
    	local_status_ = P_START;
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
    	static int count = 0;
    	static  bool flag = false;
    	static S32 lmotor,rmotor;

		
        switch(local_status_){
            // スタート時はスピードを抑え目にして安定した走りだしをする
            case P_START:
            
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
                cmd.param3 = 0;
                
                /* 終了条件 */
                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = P_STRAIGHT_BEFORE;
                    TimerStop();
                }
                break;

            // 直線走行
            case P_STRAIGHT_BEFORE:
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);

                if(Robot::Instance().sonar_distance < 30 && flag == false){
                    count++;
                }else {
                    count = 0;
                }

                if(count > 3){
                    Robot::Instance().Beep();
                    count = 0;
                    flag = true;
                    TimerStart(800);
                }
                
                /* 終了条件*/
                if(IsTimeup()){
                    Robot::Instance().Beep();
                    local_status_ = P_STOP;
                    
                    /* 初期化 */
                    flag = false;
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    TimerStart(1000);
                }
                break;

            // 停止状態
            case P_STOP:
                
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
                cmd.param3 = 90;
                
                /* 終了条件 */
                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = P_LOOKUP;
                    
                    /* 初期化 */
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    TimerStart(100);
                    Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);
                }
                break;

            // LOOKUP
            case P_LOOKUP:
            
                /* 走行計算 */ 
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
                cmd.param3 = 90;
                
                /* 終了条件　*/
                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = P_STOP_LOOKUP;
                    
                    /* 初期化 */
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    TimerStart(100);
                }
                break;

            //斜め停止
            case P_STOP_LOOKUP:
                
                /* 走行計算 */
                static int temptail = 90;
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = temptail;
                
                if (IsTimeup()) {
                    temptail--;
                    TimerStart(100);
                }
                
                /* 終了条件 */
                if(temptail == 70){
                    local_status_ = P_STRAIGHT_LOOKUP;
                    
                    /* 初期化 */
                    temptail = 90;
                    TimerStart(1000);
                }
                break;

            // 斜め停止2
            case P_STOP_LOOKUP2:
                /* 走行計算 */
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = 70;
                
                /*　終了条件 */
                if (IsTimeup()) {
                    local_status_ = P_STRAIGHT_LOOKUP;
                    Robot::Instance().Beep();
                }
                break;

            // 直線走行(lookup)
            case P_STRAIGHT_LOOKUP:
                /* 走行計算 */
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 37;
                cmd.param2 = 30;
                cmd.param3 = 68;
                
                /* 終了条件 */
                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) >  DIST_LOOKUP){
                    Robot::Instance().Beep();
                    local_status_ = P_BACK;
                    
                    /* 初期化 */
                    Robot::Instance().GetMotorEncoder(lmotor,rmotor);
                    TimerStart(2000);
                }
                break;

            // BACK
            case P_BACK:
            
                /* 走行計算 */
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = -10;
                cmd.param2 = -10;
                cmd.param3 = 105;

                /* 終了条件 */
                S32 lmotor2,rmotor2;
                Robot::Instance().GetMotorEncoder(lmotor2,rmotor2);
                if( (lmotor - lmotor2 > LOOKUPENCODER) ||( rmotor - rmotor2 > LOOKUPENCODER)){
                    Robot::Instance().Beep();
                    local_status_ = P_STRAIGHT;
                    
                    /* 初期化 */
                    Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() + 20);
                    Robot::Instance().ResetBalanceAPI();
                    Navigator::Instance().ResetEncoder();
                    TimerStart(5000);
                    speed_ = 20;
                    pid_trace_.Init();
                }
                break;

            // STAND UP
            case P_STANDUP:
            
                /* 走行計算 */
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = 108;
                
                /* 終了条件 */
                if (IsTimeup()) {
                    local_status_ = P_STRAIGHT;

                    /* 初期化 */ 
                    Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() + 20);
                    Robot::Instance().ResetBalanceAPI();
                    Navigator::Instance().ResetEncoder();
                    speed_ = 10;
                    tec_r_.Init();
                }
                break;

            case P_STRAIGHT:
            
                /* 走行計算 */
                cmd = tec_r_.Calclate(speed_, -5, edge_);
                cmd.param3 = 0;

                /* 終了条件 */
                if( evf.is_SetEvent(Robot::EVENT_LIGHT_BLACK) ){
                    local_status_ = P_STRAIGHT2;
                    ret = false;
                    
                    /* 初期化 */
                    speed_ = 10;
                    TimerStart(3000);
                    pid_trace_.Init();
                }
                break;
            case P_STRAIGHT2:
            
                /* 走行計算 */
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
                cmd.param3 = 0;

                /* 終了条件 */
                if (IsTimeup()) {
                    ret = false;
                    
                    /* 初期化 */
                    speed_ = 50;
                    TimerStop();
                }
                break;

            default:
                ret = false;
		}

        return ret;

    }



private:

	S16 speed_;

    TecPIDTrace pid_trace_;
	TecStraight straight_;
	TecRotate      tec_r_;
	Technic::DIRECTION edge_;
	LocalState local_status_;
};
