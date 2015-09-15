#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


namespace
{
	static const Vec POINTS[1] ={
		Vec(1,1)
	};

	static const int DIST_S_TURN_POINT_IN_1 = 1500000;
	static const int TAIL_ANG = 90;
	static const int BALLON_SPEED = 200;

}

//---------------------------------------------------------------------------
/*!
 * @brief   S���U���̂��߂̃~�b�V����
 * @see     
 * 
 *   ��{�I��Mission_Defalt���x�[�X�ɍ쐬���Ă���
 */
//---------------------------------------------------------------------------
class MissionBalloon : public Mission {

	typedef List<Vec> PointList;
	static bool check_distance(const Vec& lhs, const Vec& rhs)
	{
		return (lhs.sub(rhs).length()) < 5.0;
	}


public:

    MissionBalloon(S32 timer = 1000, S16 speed = 50, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail; 
		dist.x = 0;
        dist.y = 0;
        step_ = 0;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		if (timer_ > 0) {
			TimerStart(timer_);
		}
        Robot::Instance().Beep();

        dist.x = ni.pos.X;
        dist.y = ni.pos.Y;
		step_ = 0;
		speed_=80;
		//Navigator::Instance().StartDirCorrection (400.0, 180.0);
    }


    //---------------------------------------------------------------------------
	/*!
	 * @brief   Mission���s
	 * @param[in] ri ���{�b�g�̍ŐV���. �G���R�[�_�l�Ȃǂ��L�^����Ă���
	 * @param[in] ni ���W���. ���{�b�g��񂪍��W�l�ɕϊ����ꂽ�f�[�^�������Ă���
	 * @param[in] evf �C�x���g�t���O
	 * @param[out] cmd ���{�b�g�̏o�͏��. �o�͒l�͑��x�ƕ���
	 * @returns true : Mission���s, false : Mission�I��
	 * 
	 *   ������ Run �̏ڍׂ��L��.
	 */
	//---------------------------------------------------------------------------
	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		Vec now( ni.pos.X, ni.pos.Y );	// ���݈ʒu(x,y)mm


		// F32 f = now.dot( prevVec_ ); // ���Ƃ��΁A����̓x�N�g���̓��ς��v�Z���Afloat�ŕۑ�


		// cmd.param1 = speed_; �X�s�[�h������ NORMAL_MODE�Ȃ�Speed(%)[-100, 100], DIRECT_MODE�Ȃ獶���[�^��PWM�l[-100, 100]
		// cmd.param2 = direction; ���������� NORMAL_MODE�Ȃ����(%)[-100, 100], DIRECT_MODE�Ȃ�E���[�^��[PWM][-100, 100]
		// ����URL���Q�Ƃ��������Fhttp://lejos-osek.sourceforge.net/ecrobot_c_api_frame_jp.htm

		// �Ō�ɑO��ʒu�Ƃ��ĕۑ�

		prevVec_ = now;

		/////�ȉ��A�����̃R�[�h///////
		//(�s�K�v�Ȃ�����Ă�������)

       // cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, Technic::LEFT);
		
		/*if (IsTimeup()) {
			ret = false;
			Robot::Instance().Beep();
		}*/

		/*�K��������*/
		cmd.param3 = TAIL_ANG;


		if(step_ == 0){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 1){    /* �ꎞ��~ */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* �I������ */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//		|
		//		|
        else if(step_ == 2){    /* �E����(90�x) */
            cmd.param1 = 0;
            cmd.param2 = 70;
            /* �I������ */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 3){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 4){    /* �ꎞ��~ */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* �I������ */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |
		//		|
        else if(step_ == 5){    /* �E����(90�x) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* �I������ */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
		}
        else if(step_ == 6){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 7){    /* �ꎞ��~ */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* �I������ */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |
		//		|
        else if(step_ == 8){    /* �E����(90�x) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* �I������ */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 9){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 10){    /* �ꎞ��~ */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* �I������ */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |     |
		//		|     |
        else if(step_ == 11){    /* �E����(90�x) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* �I������ */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
                step_++;
            }
        }
        else if(step_ == 12){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1/2){
                TimerStart(100);
                step_++;
            }
        }
        else if(step_ == 13){    /* �ꎞ��~ */
            cmd.param1 = 0;
            cmd.param2 = 0;
            if(IsTimeup()){
            /* �I������ */
                base_ang_ = ni.pos.Ang;
                step_++;
            }
        }
		//      ______
		//      |     |
		//		|   __|
        else if(step_ == 14){    /* �E����(90�x) */
            cmd.param1 = 0;
            cmd.param2 = 50;
            /* �I������ */
            if((ni.pos.Ang - base_ang_) <= -(90.0F * M_PI) / 180.0F){
                step_++;
            }
		}
        else if(step_ == 15){    /* ���i */
            cmd.param1 = BALLON_SPEED;
            cmd.param2 = 0;
            /* �I������ */
            if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) + (ni.pos.Y - dist.y) * (ni.pos.Y - dist.y)) > DIST_S_TURN_POINT_IN_1/2){
                TimerStart(100);
                step_++;
            }
        }
		else if(step_ == 16){			/* �i���Ƀ��C���g���[�X */
			cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, Technic::LEFT);
        }

		cmd.param3 = 0;

        return ret;

    }



private:

    TecPIDTrace pid_trace_;
	Vec prevVec_;//< �O��̈ʒu(x,y)mm

	typedef struct distance{
		float x;
		float y;
		float xy;
	};
	distance dist;
	F64 base_ang_;
	S16 step_;
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_Default instance_;

};
