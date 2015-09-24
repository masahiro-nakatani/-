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
		static S16 prevRotatedDegree = 0; // �O���PID�ɂ��X��
		// inclination > 0 : ���ɌX���Ă���
		S16 inclination = Navigator::convertToMotorValue( encoderL, encoderR ) - prevRotatedDegree;	// ���{�b�g�̌X��[-100,100](�������O��̌��Z���TPID�ɂ��e���͍l���Ȃ�)
		prevRotatedDegree = -1 * PIDLightParam;	// PID�l�͐���ʂȂ̂ŁA����K�v�ʂ�-1��������
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

		// �X�^�[�g���̓X�s�[�h��}���ڂɂ��Ĉ��肵�����肾��������
		if (P_START == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ ), edge_);
			cmd.param2 += calculateMotorPID( ri.rot_encoderL_val, ri.rot_encoderR_val, cmd.param2 );
			cmd.param3 = 0;
			if (is_timeup()) {
				Robot::Instance().Beep();
				// ��ԑJ��
				local_status_ = P_STRAIGHT_BEFORE;
				//Robot::Instance().SetGyroOffset(Robot::Instance().GetGyroOffset() - 10);
				//timer_start(5000);
			}
		}

		// �������s
		else if (P_STRAIGHT_BEFORE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			cmd.param2 += calculateMotorPID( ri.rot_encoderL_val, ri.rot_encoderR_val, cmd.param2 );
			// �T���v���̂��߁A���̏�Ԃɂ͑J�ڂ��Ȃ��悤�ɃR�����g�A�E�g���Ă����܂��B

			// //if (is_timeup()) {
			// 	#ifdef IN_COURSE
			// 	if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_STRAIGHT_BEFORE_IN){
			// 	#else
			// 	if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_STRAIGHT_BEFORE_OUT){
			// 	#endif
			// 	Robot::Instance().Beep();
			// 	// ��ԑJ��
			// 	local_status_ = P_UP_SLOPE;
			// 	dist.x = ni.pos.X;
    	// 		dist.y = ni.pos.Y;
    	// 		dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
			// 	//timer_start(50000);
			// }

		}

		// �⓹�̂̂ڂ葖�s
		else if (P_UP_SLOPE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_), edge_);
			//if (is_timeup()) {
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_UP_SLOPE){
				Robot::Instance().Beep();
				// ��ԑJ��
				local_status_ = P_DOWN_SLOPE;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				//timer_start(5000);
			}
		}

		// �⓹�̂����葖�s
		else if (P_DOWN_SLOPE == local_status_) {
			cmd = pid_trace_.Calclate(ri.light_sensor_val, (S16)(speed_ * 0.9), edge_);
			//Robot::Instance().Beep();
			//if (is_timeup()) {
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_DOWN_SLOPE){
				Robot::Instance().Beep();
				// ��ԑJ��
				local_status_ = P_STRAIGHT_AFTER;
				dist.x = ni.pos.X;
    			dist.y = ni.pos.Y;
    			dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				//timer_start(5000);
			}
		}

    	// �������s(arter)
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
			//if((ni.pos.X > SHIFT_LOOKUP) && Robot::Instance().sonar_distance < 60){ //���̂����m�����玟�̃~�b�V������
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
