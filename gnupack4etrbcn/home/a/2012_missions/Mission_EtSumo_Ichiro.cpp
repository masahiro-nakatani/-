#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define TARGET_RANGE	60			// ペットボトル検知有効範囲(単位cm)
#define ET_SPEED		50
#define RO_SPEED		40
#define DIST_EDGE		10000
#define DIST_MARK		300000


#define M_PI 3.1419
#define DIST_DRIFT_TURN_TEST	300000



static const Vec MarkerPoint[] =
{
	Vec( 4166.3208,   2874.3323),		// ハート型の左突端
	Vec( 4474.9375,   2643.6416),		// ハード型の中央くぼみ
	Vec( 4855.2471,   2294.9550),		// ハート型の右突端
};



//


class TecFFFFfewsdfasdfsdfasd
{
public:
    void Init(void);
    RobotCmd  Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge);

private:
    F32 Rotation_;

    F32 E1_;
    F32 E2_;
    PIDParam PIDParam_;
};
void TecPIDTrace::Init(void){
        Rotation_ = 0.0;
        E1_ = 0.0;
        E2_ = 0.0;
//        PIDParam pid_param = { 0.38, 0.0, 0.027 };
//        PIDParam pid_param     = { 1.0, 0.0, 40.0 };
//        PIDParam_ = pid_param;

        PIDParam_ = pid_light_param;
}

RobotCmd  TecPIDTrace::Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge)
{
    F32 normalizedLightValue = (1.0 - (-1.0)) /
            (CourseInfo::colorBlack - CourseInfo::colorWhite) *
            (lightValue - CourseInfo::colorWhite) + (-1.0);

    F32 e0 = normalizedLightValue - (-0.5);     // ちょっと白より（灰色対策)

    F32 deltaRotation;

    pid_calc_rotation (&PIDParam_, e0, E1_, E2_, &deltaRotation);

    if ( edge == Technic::RIGHT ){
        Rotation_ += 50.0 * deltaRotation;
    } else {
        Rotation_ -= 50.0 * deltaRotation;
    }

    S16 accel = speed;


    RobotCmd ret((S16)accel, (S16)Rotation_);

    E2_ = E1_;
    E1_ = e0;

    return ret;

}






class Mission_EtSumo : public Mission
{

private:
	enum LocalState {
		eWAIT,
		eSTART,
		eRUN_EDGE,

		eTURN_LEFT,
		eTURN_RIGHT,
		eSEARCH_END_GO,

		eATTACK,
		eATTACK_COMPLETE1,
		eATTACK_COMPLETE2,
		eGIVEUP,
		eRECOVER1,
		eRECOVER2,
		eRECOVER3,
		eFINISH,
	};

	bool			is_startpoint(NavInfo ni)
	{
		bool ret = false;
#if 0
		Vec selfPos = Vec(ni.pos.X, ni.pos.Y);
		Vec targetPos = Vec(MarkerPoint[1]);
#else
	// デバッグ用
		Vec selfPos = Vec(ni.pos.X - m_org_pos_.X, ni.pos.Y - m_org_pos_.Y);
		Vec targetPos1 = Vec(MarkerPoint[1]);
		Vec targetPos0 = Vec(MarkerPoint[0]);
		Vec targetPos  = targetPos1.sub(targetPos0);
#endif
		Vec v = targetPos.sub(selfPos);
		F32 len = v.length();
		if (len <= 2.0) {
			ret = true;
		}
		return ret;
	}
	typedef struct	distance{
		float x;
		float y;
		float xy;
	};

	distance		m_first_dist;

public:

    Mission_EtSumo(S32 timer = 0, S16 speed = ET_SPEED, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
		m_timer_ = timer;
		m_speed_ = speed;
		m_tail_  = tail;
    }


    virtual void	Init(RobotInfo ri, NavInfo ni)
	{
        m_tec_pid_.Init();
        m_tec_s_.Init(ri);
		m_tec_r_.Init();

		m_is_state_move_	= true;
		m_edge_				= Technic::LEFT;
		m_speed_			= ET_SPEED;
		m_local_status_		= eWAIT;
//		m_local_status_		= eRUN_EDGE;
		m_preLock			= 0;
		m_attackTime		= 0;
		m_org_pos_			= ni.pos;
		if (m_timer_ > 0)
		{
			TimerStart(m_timer_);
		}
    	m_first_dist.x				= ni.pos.X;
    	m_first_dist.y				= ni.pos.Y;
    	m_first_dist.xy				= (m_first_dist.x * m_first_dist.x) + (m_first_dist.y * m_first_dist.y);

    }
    virtual bool	Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd )
	{

        bool ret = true;

		// ソナーで距離を測る
		m_nowDistance = Robot::Instance().GetSonarDistance();


		switch (m_local_status_)
		{
			// 開始位置待ち
		case eWAIT:

			m_speed_	= ET_SPEED;
			cmd			= m_tec_pid_.Calclate(ri.light_sensor_val, m_speed_, m_edge_);

			float a_tX		= ni.pos.X - m_first_dist.x;
			float a_tY		= ni.pos.Y - m_first_dist.y;
			float a_tDist	= a_tX * a_tX + a_tY * a_tY;

			if( a_tDist > DIST_DRIFT_TURN_TEST)//一定の距離を超えたら
			{
				m_local_status_ = eTURN_LEFT;
			}

			break;



			// 左半回転しペットボトルを探す
		case eTURN_LEFT:

			if (m_is_state_move_) //最初の１回だけ実行
			{
				m_is_state_move_	= false;
				m_turn_status_		= eTURN_LEFT;
				m_posBase2			= ni.pos;
				m_edge_				= Technic::LEFT;
				cmd					= m_tec_r_.Calclate(0, 0, m_edge_);
				m_preLock			= 0;
			}
			else
			{
				// 90度左回転で探す
				m_speed_ = 0;
				cmd = m_tec_r_.Calclate(m_speed_, 50, m_edge_);


				if( (m_nowDistance < m_forwardDistance) && (m_nowDistance < TARGET_RANGE) )
				{
					// 60cm以内にペットボトルを発見
					TimerStart(100);
					m_preLock = 1;
				}

				if( IsTimeup() && (m_nowDistance > m_forwardDistance) && m_forwardDistance < 200)
				{
					m_local_status_ = eSEARCH_END_GO;//次の状態へ移動
					m_posBase3		= ni.pos;
					Robot::Instance().Beep(Robot::A);
				}

				if( (ni.pos.Ang - m_posBase2.Ang) > 0.6 * M_PI )
				{
					// 45度左回転して見つからなかった場合、右回転へ
					m_is_state_move_	= true;
					m_local_status_		= eTURN_RIGHT;//右旋回へ移動
					m_edge_				= Technic::RIGHT;

				}
			}
			break;

			// 右半回転しペットボトルを探す
		case eTURN_RIGHT:

			if (m_is_state_move_)
			{
				m_is_state_move_	= false;
				m_turn_status_		= eTURN_RIGHT;
				m_edge_				= Technic::RIGHT;
				cmd					= m_tec_r_.Calclate(0, 0, m_edge_);
				m_preLock			= 0;
			}
			else
			{
				// 90度右回転で探す
				m_speed_	= 0;
				cmd			= m_tec_r_.Calclate(m_speed_, 50, m_edge_);

				if( (m_nowDistance < m_forwardDistance) && (m_nowDistance < TARGET_RANGE) )
				{
					// 60cm以内にペットボトルを発見
					//TimerStart(100);
					m_preLock = 1;
				}

				if( (m_preLock == 1) && (m_nowDistance > m_forwardDistance) )
				{
					m_local_status_ = eSEARCH_END_GO;//次の状態へ移動
					m_posBase3		= ni.pos;
					Robot::Instance().Beep(Robot::A);
				}

				if( (m_posBase2.Ang - ni.pos.Ang) > 0.5 * M_PI )
				{
					// 90度右回転して見つからなかった場合、あきらめる
					m_is_state_move_ = true;
					m_preLock = 0;
					m_local_status_ = eGIVEUP;
				}
			}
			break;


		case eSEARCH_END_GO:

			m_speed_	= ET_SPEED;
			cmd			= m_tec_pid_.Calclate(ri.light_sensor_val, m_speed_, m_edge_);

			float a_tX		= ni.pos.X - m_first_dist.x;
			float a_tY		= ni.pos.Y - m_first_dist.y;
			float a_tDist	= a_tX * a_tX + a_tY * a_tY;

			if( a_tDist > DIST_DRIFT_TURN_TEST)//一定の距離を超えたら
			{
				m_local_status_ = eTURN_LEFT;
			}

			break;



			// ET相撲実行中
		case eATTACK:

			if (m_is_state_move_)
			{
				m_attackTime = 0;
				m_is_state_move_ = false;
			}
			else
			{
				m_attackTime++;
			}
			m_speed_ = ET_SPEED;

			if( (m_nowDistance > m_forwardDistance) || m_nowDistance > 200)
			{
				m_tec_r_.Init();
				cmd = m_tec_r_.Calclate(m_speed_, 10, m_edge_);
				Robot::Instance().Beep();
			}
			else
			{
				m_tec_s_.Init(ri);
				cmd = m_tec_s_.Calclate(m_speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			}

			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) )
			{
				// 黒ラインに到着したらET相撲実行を終了し、復帰へ
				m_is_state_move_ = true;
				m_local_status_ = eATTACK_COMPLETE1;
				Robot::Instance().Beep(Robot::B);
			}
			break;


		case eATTACK_COMPLETE1:

			if(m_is_state_move_)
			{
				TimerStart(2000);
				m_is_state_move_ = false;
			}

			m_speed_	= -ET_SPEED;
			m_tec_s_.Init(ri);
			cmd			= m_tec_s_.Calclate(m_speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);

			if(IsTimeup())
			{
				m_local_status_		= eATTACK_COMPLETE2;
				m_is_state_move_	= true;
				Robot::Instance().Beep(Robot::A2);
			}
			break;


		case eATTACK_COMPLETE2:
			if(m_is_state_move_)
			{
				// 土俵際へ移動したときと同じ向きになるまで回転する
				m_speed_ = 0;
				if( m_turn_status_ == eTURN_RIGHT )
				{
					cmd = m_tec_r_.Calclate(m_speed_, 50, m_edge_);

					if( (ni.pos.Ang - m_posBase3.Ang) >= 0.0 )
					{
						m_is_state_move_ = false;
						Robot::Instance().Beep(Robot::A2);
					}

				}
				else
				{	// m_turn_status_ == eTURN_LEFT
					cmd = m_tec_r_.Calclate(m_speed_, -50, m_edge_);

					if( (ni.pos.Ang - m_posBase3.Ang) <= 0.0 )
					{
						m_is_state_move_ = false;
						Robot::Instance().Beep(Robot::A2);
					}

				}
			}
			else {
				m_speed_ = -ET_SPEED;
				m_tec_s_.Init(ri);
				cmd = m_tec_s_.Calclate(m_speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
				if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
					m_is_state_move_ = true;
					m_local_status_ = eRECOVER1;
					Robot::Instance().Beep(Robot::A2);
				}
			}
			break;


		case eGIVEUP:
			// ET相撲をあきらめた場合は90度左回転後にバックでレーンに復帰
			if (m_is_state_move_) {
				m_speed_ = 0;
				cmd = m_tec_r_.Calclate(m_speed_, 50, m_edge_);
				if( (ni.pos.Ang - m_posBase2.Ang) > 0.0 ){
					m_is_state_move_ = false;
				}
			}
			else {
				m_speed_ = -ET_SPEED;
				m_tec_s_.Init(ri);
				cmd = m_tec_s_.Calclate(m_speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
				if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
					m_is_state_move_ = true;
					m_local_status_ = eRECOVER1;
					Robot::Instance().Beep(Robot::A2);
				}
			}
			break;


		case eRECOVER1:			// 方向転換
			// 60度だけ左回転する
			m_speed_ = 0;
			cmd = m_tec_r_.Calclate(m_speed_, 40, m_edge_);
			if( (ni.pos.Ang - m_posBase1.Ang) > 0.33 * M_PI ){
				m_is_state_move_ = true;
				m_local_status_ = eRECOVER2;
			}
			break;


		case eRECOVER2:			// 少し直進
			if (m_is_state_move_) {
				m_is_state_move_ = false;
				m_tec_s_.Init(ri);
			}
			m_speed_ = 20;
			cmd = m_tec_s_.Calclate(m_speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
				m_local_status_ = eRECOVER3;
				m_tec_pid_.Init();
				m_is_state_move_ = true;
				Robot::Instance().Beep(Robot::G);
			}
			break;


		case eRECOVER3:			// しばらくライントレースし、ミッション終了する
			if (m_is_state_move_) {
				m_tec_pid_.Init();
				m_is_state_move_ = false;
				TimerStart(5000);
			}
			m_speed_ = ET_SPEED;
			cmd = m_tec_pid_.Calclate(ri.light_sensor_val, m_speed_, m_edge_);
			if(IsTimeup()){
				m_local_status_ = eFINISH;
				Robot::Instance().Beep(Robot::F);
				ret = false;
			}
			break;

		default:
			ret = false; // ミッションを終了？
			break;

		}

		m_forwardDistance	= m_nowDistance;
		cmd.param3			= 0;

        return ret;

    }


private:

    TecPIDTrace			m_tec_pid_;
    TecStraight			m_tec_s_;
	TecRotate			m_tec_r_;

	Technic::DIRECTION	m_edge_;

	S32					m_timer_;
	S16					m_speed_;
	S16					m_tail_;
    LocalState			m_local_status_;
    LocalState			m_turn_status_;
	bool				m_is_state_move_;
	Position			m_org_pos_;
	Position			m_posBase1;
	Position			m_posBase2;
	Position			m_posBase3;
	S16					m_nowDistance;
	S16					m_forwardDistance;
	int					m_preLock;
	int					m_attackTime;

//    static Mission_EtSumo instance_;

};

//Mission_EtSumo Mission_EtSumo::instance_;
