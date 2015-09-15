#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class Mission_CourseG1_LookupGate : public Mission {
private:
	enum LocalState {
		P_RUN_START,
		P_RUN_STOP,
		P_LOOKUP_START,
		P_LOOKUP_STOP,
		P_RUN_AGAIN,
	};

	enum LocalDifinitions {
		DIST_LOOKUP	= 100000,
	};

	typedef struct distance {
		float x;
		float y;
		float xy;
	};


public:
    Mission_CourseG1_LookupGate(S32 timer, S16 speed, S16 tail, Technic::DIRECTION edge)
		: Mission()
	{
		timer_ = timer;
		speed_ = speed;
		tail_ = tail;
		edge_ = edge;
	}

    virtual void Init(RobotInfo ri, NavInfo ni)
	{
        pid_trace_.Init();
		if (0 < timer_) {
			TimerStart(timer_);
		}
    	local_status_ = P_RUN_START;
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd)
	{
        bool ret = true;
		
        switch (local_status_) {
            case P_RUN_START:	// 単純走行.
                // 走行計算.
                cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
                cmd.param3 = tail_;
                // 終了条件：指定時間で終了.
                if (IsTimeup()) {
                    Robot::Instance().Beep(Robot::E);
                    local_status_ = P_RUN_STOP;
                    TimerStart(1000);
                }
                break;

            case P_RUN_STOP:	// 走行停止(ゲート通過準備).
                // 走行計算.
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
                cmd.param3 = 90;	// シッポON.
                // 終了条件：指定時間で終了.
                if (IsTimeup()) {
                    Robot::Instance().Beep(Robot::F);
                    local_status_ = P_LOOKUP_START;
                    // 位置情報の初期化.
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    TimerStart(1000);
					// 躯体を20度仰向きに反らす.
                    Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);
                }
                break;

            case P_LOOKUP_START:	// ゲート通過.
                // 走行計算.
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
                cmd.param3 = 90;	// シッポON.
                // 終了条件：指定時間で終了.
                if (IsTimeup()) {
                    Robot::Instance().Beep(Robot::G);
                    local_status_ = P_LOOKUP_STOP;
                    // 位置情報の初期化.
                    dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
                    TimerStart(1000);
                }
                break;

            case P_LOOKUP_STOP:		// ゲート終了.
                // 躯体の傾き調整(少し上体を起こす).
                static int temptail = 90;
                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = temptail;
                
                if (IsTimeup()) {
                    temptail--;
                    TimerStart(100);
                }
                
                // 終了条件：躯体の傾きが指定値になったら終了.
                if (temptail == 70){
                    local_status_ = P_RUN_AGAIN;
                    // 初期化.
                    temptail = 90;
                    TimerStart(1000);
                }
                break;

            case P_RUN_AGAIN:		// 走行再開.
                // 走行計算.
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
                cmd.param3 = 68;
                // 終了条件：一定距離進んだら終了.
				F64 dx = ni.pos.X - dist.x;
				F64 dy = ni.pos.Y - dist.y;
                if (DIST_LOOKUP < (dx * dx + dy * dy)) {
                    Robot::Instance().Beep(Robot::A);

					ret = false;
                    TimerStart(1000);
                }
                break;
		}

        return ret;
    }


    virtual char* Name(void){
        return "Mission_CourseG1_LookupGate";
    }


private:
	S32 timer_;
	S16 speed_;
	S16 tail_;
	Technic::DIRECTION edge_;

	TecPIDTrace pid_trace_;
	distance dist;
	LocalState local_status_;
};
