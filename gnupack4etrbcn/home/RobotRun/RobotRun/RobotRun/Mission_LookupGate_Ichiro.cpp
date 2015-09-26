#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
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
		P_STANDUP,//“ü‚ç‚È‚¢Žq
		P_STRAIGHT,
		P_STRAIGHT2
	};

	enum LocalDifinitions {
		DIST_STRAIGHT_BEFORE_OUT	= 100000,
		DIST_STRAIGHT_BEFORE_IN		= 3287729,
		DIST_UP_SLOPE				= 350000,
		DIST_DOWN_SLOPE				= 1392616,
		DIST_LOOKUP					= 100000	
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
		timer_start(1000);
		speed_ = 70;
    	local_status_ = P_START;
		tec_pid_.Init();
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
    	static int count = 0;
    	static  bool flag = false;
    	static S32 lmotor,rmotor;


		// ƒXƒ^[ƒg
		if (P_START == local_status_) {
			int dis_gate = 10000;

			cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(80), edge_);
			if (Soner(dis_gate) == true) {
				local_status_ = P_LOOKUP;
				Robot::Instance().Beep();
			}
		}

		else if(P_STOP == local_status_){
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
			cmd.param3 = 90;

			//ƒ^ƒCƒ}[‚Ç‚¤‚µ‚æ‚¤H
			if (is_timeup()) {
				local_status_ = P_LOOKUP;
				Robot::Instance().Beep();
				timer_start(100);
				Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);
			}
		}

		// LOOKUP
		else if (P_LOOKUP == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
			cmd.param3 = 90;
			if (is_timeup()) {
				Robot::Instance().Beep();
				// ó‘Ô‘JˆÚ
				local_status_ = P_STOP_LOOKUP;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				timer_start(100);
			}
		}
		
		//ŽÎ‚ß’âŽ~
		else if (P_STOP_LOOKUP == local_status_) {
			static int temptail = 90;
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 0;
			cmd.param2 = 0;
			cmd.param3 = temptail;
			if (is_timeup()) {
				temptail--;
				timer_start(100);
			}
			if(temptail == 70){
				local_status_ = P_STRAIGHT_LOOKUP;
				temptail = 90;
				// ó‘Ô‘JˆÚ
				timer_start(1000);
			}
		}
    	
		// ŽÎ‚ß’âŽ~2
		else if (P_STOP_LOOKUP2 == local_status_) {
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 0;
			cmd.param2 = 0;
			cmd.param3 = 70;
			if (is_timeup()) {
				local_status_ = P_STRAIGHT_LOOKUP;
				Robot::Instance().Beep();
				// ó‘Ô‘JˆÚ
				//timer_start(5000);
			}
		}
			
    	// ’¼ü‘–s(lookup)
		else if (P_STRAIGHT_LOOKUP == local_status_) {
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 37;
			cmd.param2 = 30;
			cmd.param3 = 68;
			
			
				
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) >  DIST_LOOKUP){
				// ó‘Ô‘JˆÚ
				local_status_ = P_BACK;
				Robot::Instance().GetMotorEncoder(lmotor,rmotor);
				Robot::Instance().Beep();
				timer_start(2000);
			}
		}
			
		// BACK
		else if (P_BACK == local_status_) {
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = -10;
			cmd.param2 = -10;
			cmd.param3 = 105;
			S32 lmotor2,rmotor2;
			Robot::Instance().GetMotorEncoder(lmotor2,rmotor2);
			//if (is_timeup()) {
				if( (lmotor - lmotor2 > LOOKUPENCODER) ||( rmotor - rmotor2 > LOOKUPENCODER)){
				// ó‘Ô‘JˆÚ
				local_status_ = P_STRAIGHT;
				Robot::Instance().Beep();
				Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() + 20);
				Robot::Instance().ResetBalanceAPI();
				Navigator::Instance().ResetEncoder();
				timer_start(5000);
				speed_ = 20;
				pid_trace_.Init();
			}
		}
			
		// STAND UP
		else if (P_STANDUP == local_status_) {
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 0;
			cmd.param2 = 0;
			cmd.param3 = 108;
			if (is_timeup()) {
				// ó‘Ô‘JˆÚ
				local_status_ = P_STRAIGHT;
				Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() + 20);
				Robot::Instance().ResetBalanceAPI();
				Navigator::Instance().ResetEncoder();
				//timer_start(5000);
				speed_ = 10;
				//pid_trace_.Init();
				tec_r_.Init();
			}
		}


		else if (P_STRAIGHT == local_status_) {
			cmd = tec_pid_.Calclate(ri.light_sensor_val, (S16)(80), edge_);
			cmd.param3 = 108;
			
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
				local_status_ = P_STRAIGHT2;
				speed_ = 10;
				timer_start(3000);
				pid_trace_.Init();
				//flag = true;
				ret = false;
				//timer_stop();
			}
		}

		else {
			ret = false;
		}

        return ret;

    }

	//’´‰¹”gƒZƒ“ƒT‚ªè‡’l‚É’B‚µ‚½‚çtrue‚ð•Ô‚·
	virtual bool Soner(int dist_gate) {
		nowDistance = Robot::Instance().GetSonarDistance();
		if (nowDistance >= dist_gate)	return true;
		else return false;
	}




private:

	S16 speed_;
	S16 nowDistance;
	
    TecPIDTrace pid_trace_;
	TecStraight straight_;
	TecPIDTrace tec_pid_;
	TecRotate      tec_r_;
	Technic::DIRECTION edge_;
	LocalState local_status_;
};

