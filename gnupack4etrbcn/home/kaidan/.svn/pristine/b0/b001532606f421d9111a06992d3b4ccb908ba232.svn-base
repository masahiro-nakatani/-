#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define SPEED_LOW				50
#define SPEED_TOP				100
#define TURN_RIGHT				5
#define TURN_LEFT				-5

class MissionStart : public Mission {
private:
	enum LocalState {
		P_UP_TAIL,
		P_GO_RIGHT,
		P_GO_LEFT,
		P_SPEED_UP,
	};
	
public:

	MissionStart(Technic::DIRECTION edge = Technic::LEFT) : Mission() {
		m_edge_ = edge;
	}
	
	virtual void Init(RobotInfo ri, NavInfo ni){
		m_pid_trace_.Init();
		m_speed_ = SPEED_LOW;
		m_local_status_ = P_UP_TAIL;
	}
	
	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd){
		bool ret = true;
		
		/* 走行計算 */
		cmd = m_pid_trace_.Calclate(ri.light_sensor_val, (S16)(m_speed_ ), m_edge_);
		
		switch(m_local_status_){
			// 尻尾を上げる
			case P_UP_TAIL:
				Robot::Instance().ResetBalanceAPI();
				cmd.Mode = RobotCmd::NORMAL_MODE;
				cmd.param3 = 0;							// 尻尾を上げる
				m_posi_st_ = ni.pos;					// 開始時の位置情報を保存
				if(m_edge_ == Technic::RIGHT) {
					m_local_status_ = P_GO_RIGHT;
				}
				else {
					m_local_status_ = P_GO_LEFT;
				}
				break;
				
			// 右方向に前進する
			case P_GO_RIGHT:
				cmd.param2 = TURN_RIGHT;
				/* 終了条件 */
				if((m_posi_st_.Y - ni.pos.Y) > 5.0)		// 右エッジの右側5mmを越えた
				{
					TimerStart(500);
					m_pid_trace_.Init();
					m_local_status_ = P_SPEED_UP;
				}
				break;
				
			// 左方向に前進する
			case P_GO_LEFT:
				cmd.param2 = TURN_LEFT;
				/* 終了条件 */
				if((m_posi_st_.Y - ni.pos.Y) < -5.0)	// 左エッジの左側5mmを越えた
				{
					TimerStart(500);
					m_pid_trace_.Init();
					m_local_status_ = P_SPEED_UP;
				}
				break;
				
			// 加速する
			case P_SPEED_UP:
				if(IsTimeup()){
					m_speed_ += 10;						// 徐々にスピードを上げる
					TimerStart(200);
				}
				/* 終了条件 */
				if(m_speed_ > SPEED_TOP){
					ret = false;						// ミッション終了
				}
				break;
				
			default:
				ret = false;							// ミッション終了
				break;
			}
		
		return ret;
	}
	
private :
	S16 m_speed_;
	TecPIDTrace m_pid_trace_;
	Technic::DIRECTION m_edge_;
	Position m_posi_st_;
	LocalState m_local_status_;
};
