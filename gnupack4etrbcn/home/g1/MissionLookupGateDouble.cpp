#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define LIGHT_OFFSET	(-0.5)

class Mission_LookupGate : public Mission {
private:
	enum LocalState {
		STATE_START,
		STATE_GATE_SEARCH,
		STATE_STOP,
		STATE_INCLINE_BEHIND,
		STATE_LOOKUP_FORWARD_FIRST,
		STATE_LOOKUP_BACKWARD,
		STATE_BACKWARD_STOP,
		STATE_LOOKUP_FORWARD_SECOND,
		STATE_STANDUP,
		STATE_LOOKUP_END,
		STATE_COMEBACK_LINE,
		STATE_MISSION_END,
	};

	enum LocalDifinitions {
		DIST_LOOKUP				 = 180000,
		DIST_LOOKUP_SECOND		 = 280000,
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
		//score_ = AKB48;
	}

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
    	straight_.Init(ri);
		TimerStart(1000);
		speed_ = 30;
    	local_status_ = STATE_START;
    	dist.x = ni.pos.X;
    	dist.y = ni.pos.Y;
    	dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
    	static int count = 0;        
    	static  bool isFoundGate = false;
    	static S32 lmotor,rmotor;    

        switch(local_status_){

            case STATE_START:
            

                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_, LIGHT_OFFSET);
                cmd.param3 = 0;
                
                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = STATE_GATE_SEARCH;
                    TimerStop();
                }
                break;

            case STATE_GATE_SEARCH:

                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_, LIGHT_OFFSET);
				cmd.param3 = 0;

                if(Robot::Instance().sonar_distance < 30 && isFoundGate == false){
                    count++;
                }else {
                    count = 0;
                }
				
                if(count > 4){ 
                    Robot::Instance().Beep();
                    count = 0;
                    isFoundGate = true;
                    TimerStart(800);
                }

                if(IsTimeup()){
                    Robot::Instance().Beep();
					local_status_ = STATE_STOP;
					Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);

					//STATE_STOP Time
                    TimerStart(150);
                }
             break;

            case STATE_STOP:

                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(0), edge_, LIGHT_OFFSET);
				cmd.param3 = 90;

                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    local_status_ = STATE_INCLINE_BEHIND;
                    
					//STATE_INCLINE_BEHIND Time
                    TimerStart(1000);
                }
                break;

			case STATE_INCLINE_BEHIND:

                static int temptail = 90;

                cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = temptail;
                
                if (IsTimeup()) {
                    temptail--;
                    TimerStart(100);
                }
                
                if(temptail == 65){

					dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);

                    local_status_ = STATE_LOOKUP_FORWARD_FIRST;

  					//STATE_LOOKUP_FORWARD_FIRST Time               
                    TimerStart(1000);

                }
                break;

			case STATE_LOOKUP_FORWARD_FIRST:

				cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 37;
                cmd.param2 = 35;
                cmd.param3 = 65;

                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) >  DIST_LOOKUP){
                    Robot::Instance().Beep();
                    local_status_ = STATE_LOOKUP_BACKWARD;
                    
                    Robot::Instance().GetMotorEncoder(lmotor,rmotor);

					dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);

					//STATE_LOOKUP_BACKWARD Time     
                    TimerStart(2000);
                }
                break;

			case STATE_LOOKUP_BACKWARD:

				cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = -37;
                cmd.param2 = -37;
                cmd.param3 = 65;

                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) >  DIST_LOOKUP){
                    Robot::Instance().Beep();
                    local_status_ = STATE_BACKWARD_STOP;

					//STATE_BACKWARD_STOP Time  
                    TimerStart(1000);
                }
                break;

			case STATE_BACKWARD_STOP:

				cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
                cmd.param3 = 65;

                if (IsTimeup()) {                  
					Robot::Instance().Beep();
                    local_status_ = STATE_LOOKUP_FORWARD_SECOND;

					dist.x = ni.pos.X;
                    dist.y = ni.pos.Y;
                    dist.xy = (dist.x * dist.x) + (dist.y * dist.y);

                }
                break;

			case STATE_LOOKUP_FORWARD_SECOND:

				cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 37;
                cmd.param2 = 31;
                cmd.param3 = 65;

                if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_LOOKUP_SECOND){
                    
					//Robot::Instance().Beep();
				    local_status_ = STATE_LOOKUP_END;
					cmd.param3 = 108;

					//LOOKUP_END Time  
                    TimerStart(3000);
                }
                break;

			case STATE_LOOKUP_END:

				cmd.Mode = RobotCmd::DIRECT_MODE;
                cmd.param1 = 0;
                cmd.param2 = 0;
				cmd.param3 = 108;
				//Robot::Instance().Beep(Robot::D);

                if (IsTimeup()) {

                    local_status_ = STATE_STANDUP;
                    Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() + 20);
                    Robot::Instance().ResetBalanceAPI();
                    Navigator::Instance().ResetEncoder();
					pid_trace_.Init();
					cmd.param3 = 0;
					speed_ = 10;

					//STATE_STANDUP Time
                    TimerStart(150);
                }
                break;

            case STATE_STANDUP:
				cmd = tec_r_.Calclate(speed_, 0, edge_);
                cmd.param3 = 0;
     
                if (IsTimeup()) {

                    local_status_ = STATE_COMEBACK_LINE;
					pid_trace_.Init();

                    speed_ = 10;

					//STATE_COMEBACK_LINE Time
                    TimerStart(8000);
                }
                break;

            case STATE_COMEBACK_LINE:
            
                cmd = tec_r_.Calclate(speed_, -5, edge_);
                cmd.param3 = 0;

                /* èIóπèåè */
                if( evf.is_SetEvent(Robot::EVENT_MARKER) ){
                    local_status_ = STATE_MISSION_END;
                    
                    /* èâä˙âª */
                    speed_ = 50;
                    TimerStart(5000);
                    pid_trace_.Init();
                }
                break;

            case STATE_MISSION_END:
            
                cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_, LIGHT_OFFSET);
                cmd.param3 = 0;

                if (IsTimeup()) {
                    Robot::Instance().Beep();
                    ret = false;                
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
	U8 *score_;
};
