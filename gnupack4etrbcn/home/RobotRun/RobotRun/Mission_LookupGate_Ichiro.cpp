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
		P_STANDUP,//ì¸ÇÁÇ»Ç¢éq
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
		timer_start(1000);
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

		// ÉXÉ^Å[ÉgéûÇÕÉXÉsÅ[ÉhÇó}Ç¶ñ⁄Ç…ÇµÇƒà¿íËÇµÇΩëñÇËÇæÇµÇÇ∑ÇÈ
		if (P_START == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			cmd.param3 = 0;
			if (is_timeup()) {
				Robot::Instance().Beep();
				// èÛë‘ëJà⁄
				local_status_ = P_STRAIGHT_BEFORE;
				timer_stop();
				//timer_start(5000);
			}
		}

		// íºê¸ëñçs
		else if (P_STRAIGHT_BEFORE == local_status_) {
			
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
				timer_start(800);
			}
			if(is_timeup()){
			//if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_STRAIGHT_BEFORE_OUT){
				Robot::Instance().Beep();
				flag = false;
				// èÛë‘ëJà⁄
				local_status_ = P_STOP;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				timer_start(1000);
			}
		}

		// í‚é~èÛë‘
		else if (P_STOP == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
			cmd.param3 = 90;
			if (is_timeup()) {
				Robot::Instance().Beep();
				// èÛë‘ëJà⁄
				local_status_ = P_LOOKUP;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
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
				// èÛë‘ëJà⁄
				local_status_ = P_STOP_LOOKUP;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				timer_start(100);
			}
		}
		
		//éŒÇﬂí‚é~
		else if (P_STOP_LOOKUP == local_status_) {
			//cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
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
				// èÛë‘ëJà⁄
				timer_start(1000);
			}
		}
    	
		// éŒÇﬂí‚é~2
		else if (P_STOP_LOOKUP2 == local_status_) {
			//cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_);
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 0;
			cmd.param2 = 0;
			cmd.param3 = 70;
			if (is_timeup()) {
				local_status_ = P_STRAIGHT_LOOKUP;
				Robot::Instance().Beep();
				// èÛë‘ëJà⁄
				//timer_start(5000);
			}
		}
			
    	// íºê¸ëñçs(lookup)
		else if (P_STRAIGHT_LOOKUP == local_status_) {
			//cmd = pid_trace_lookup_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			//cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(1), edge_);
			//Robot::Instance().SetGyroOffset(Robot::Instance().gyro_data);
			cmd.Mode = RobotCmd::DIRECT_MODE;
			cmd.param1 = 37;
			cmd.param2 = 30;
			cmd.param3 = 68;
			
			
			//if (is_timeup()) {
			/*if(Robot::Instance().sonar_distance > 200){
				count++; 
			}else{
				count = 0;
			}*/
				
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) >  DIST_LOOKUP){
				// èÛë‘ëJà⁄
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
				// èÛë‘ëJà⁄
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
				// èÛë‘ëJà⁄
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
		// STRAIGHT
		/*else if (P_STRAIGHT == local_status_) {
			cmd = straight_.Calclate((S16)(speed_), ri. rot_encoderL_val, ri. rot_encoderR_val);
			cmd.param3 = 0;
		}*/
		else if (P_STRAIGHT == local_status_) {
			//if(!flag){
				cmd = tec_r_.Calclate(speed_, -5, edge_);
			//}else{
			//	cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			////	Robot::Instance().Beep();
			//}
			cmd.param3 = 0;
			
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
		else if (P_STRAIGHT2 == local_status_) {
			//if(!flag){
				cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			//}else{
			//	cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			////	Robot::Instance().Beep();
			//}
			cmd.param3 = 0;
			
			if (is_timeup()) {
				speed_ = 50;
				//flag = true;
				ret = false;
				timer_stop();
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
	TecStraight straight_;
	TecRotate      tec_r_;
	Technic::DIRECTION edge_;
	LocalState local_status_;
};

