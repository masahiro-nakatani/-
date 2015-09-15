#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define LIGHT_OFFSET	(-0.5)

class Mission_GarageIn : public Mission {
private:
	enum LocalState {
		MISSION_START,
		UNTIL_MARKER,
		PAUSE,
		TURN,
		GARAGE_IN,
		STOP,
	};
	
	Position savedPos;

public:

	Mission_GarageIn() : Mission() {
		if (CourseInfo::type == CourseInfo::InCourse) {
			edge_ = Technic::RIGHT;
		} else {
			edge_ = Technic::LEFT;
		}
	}

	virtual void Init(RobotInfo ri, NavInfo ni){
		tec_pid_.Init();
		TimerStart(2000);
		speed_ = 50;
		local_status_ = MISSION_START;
	}


	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		bool stay_mission = true;

		switch (local_status_) {
		
			// �~�b�V�����J�n(2�b�ԗ������s)
			case MISSION_START:
				/* ���s�v�Z*/
				cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_, LIGHT_OFFSET);
				cmd.param3 = 0;
				/* �I������ */
				if(IsTimeup()){
					local_status_ = UNTIL_MARKER;
				}
				break;
				
			// �}�[�J�[���m�҂�
			case UNTIL_MARKER:
				/* ���s�v�Z*/
				cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_, LIGHT_OFFSET);
				cmd.param3 = 0;
				
				/* �I������*/
				if (evf.is_SetEvent(Robot::EVENT_MARKER)) {
					cmd.param3 = 90;			// �����ۉ��낷
					TimerStart(1000);
					Robot::Instance().Beep(Robot::C);
					local_status_ = PAUSE;
				}
				break;
				
			// �ꎞ��~
			case PAUSE:
				/* ���s�v�Z */
				speed_ = -10;
				cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
				
				/* �I������ */
				if(IsTimeup()){
					Robot::Instance().Beep(Robot::D);
					Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 20);
					savedPos = ni.pos;			// ���݈ʒu��ۑ�
					local_status_ = TURN;
				}
				break;
				
			// ����]
			case TURN:
				cmd.Mode = RobotCmd::DIRECT_MODE;
				if(edge_ == Technic::RIGHT){
					/* ���s�v�Z */
					cmd.param1 = 50;			// �E��]
					cmd.param2 = -50;
					cmd.param3 = 90;
					
					/* �I������ */
					if((ni.pos.Ang - savedPos.Ang) <= -(M_PI + 0.05)){
						Robot::Instance().Beep(Robot::E);
						savedPos = ni.pos;			// ���݈ʒu��ۑ�
						local_status_ = GARAGE_IN;
					}
				}
				else
				{
					/* ���s�v�Z */
					cmd.param1 = -50;			// ����]
					cmd.param2 = 50;
					cmd.param3 = 90;
					
					/* �I������ */
					if((ni.pos.Ang - savedPos.Ang) >= (M_PI + 0.05)){
						Robot::Instance().Beep(Robot::E);
						savedPos = ni.pos;			// ���݈ʒu��ۑ�
						local_status_ = GARAGE_IN;
					}
				}
				break;
			
			// �K���[�W����
			case GARAGE_IN:
				/* ���s�v�Z */
				cmd.Mode = RobotCmd::DIRECT_MODE;
				cmd.param1 = -20;
				cmd.param2 = -20;
				cmd.param3 = 90;
				
				/* �I������ */
				if(((ni.pos.X - savedPos.X) * (ni.pos.X - savedPos.X) + (ni.pos.Y - savedPos.Y) * (ni.pos.Y - savedPos.Y)) > (40.0 * 40.0)){
					Robot::Instance().Beep(Robot::F);
					local_status_ = STOP;
				}
				break;

			// �K���[�W���(���S��~)
			case STOP:
				/* ���s�v�Z */
				cmd.Mode = RobotCmd::DIRECT_MODE;
				cmd.param1 = 0;
				cmd.param2 = 0;
				cmd.param3 = 90;
				break;

			default:
				stay_mission = false;
				break;
		}

		return stay_mission;
	}

private:

	TecPIDTrace    tec_pid_;
	Technic::DIRECTION edge_;
	S16 speed_;

	LocalState local_status_;

};
