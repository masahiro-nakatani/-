#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "Debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define TARGET_RANGE    40          // ペットボトル検知有効範囲(単位cm)
#define ET_SPEED 50
#define RO_SPEED -40
//#define DIST_CENTER 180000
#define DIST_CENTER1 90000
#define DIST_CENTER2 80000
#define DIST_CENTER3 90000
//#define DIST_MARK 300000
#define DIST_MARK 180000
//#define DIST_MARK 200000

static const Vec MarkerPoint[] = {
	Vec( 4166.3208,   2874.3323),       // ハート型の左突端
	Vec( 4474.9375,   2643.6416),       // ハード型の中央くぼみ
	Vec( 4855.2471,   2294.9550),       // ハート型の右突端
};

class Mission_EtSumo : public Mission {
private:
	enum LocalState {
		eWAIT,
		eSTART,
		eRUN_CENTER,
		eTURN,
		eATTACK,
		eATTACK_COMPLETE,
		eGOAL,
		eGIVEUP1,
		eGIVEUP2,
		eGIVEUP3,
		eRECOVER1,
		eRECOVER2,
		eRECOVER3,
		eFINISH,
		eSTRAIGHT,
	};

	bool is_startpoint(NavInfo ni)
	{
		bool ret = false;
#if 0
		Vec selfPos = Vec(ni.pos.X, ni.pos.Y);
		Vec targetPos = Vec(MarkerPoint[1]);
#else
		// デバッグ用
		Vec selfPos = Vec(ni.pos.X - org_pos_.X, ni.pos.Y - org_pos_.Y);
		Vec targetPos1 = Vec(MarkerPoint[1]);
		Vec targetPos0 = Vec(MarkerPoint[0]);
		Vec targetPos  = targetPos1.sub(targetPos0);
#endif
		Vec v = targetPos.sub(selfPos);
		F32 len = v.length();
		if (len <= 100.0) {
			Robot::Instance().Beep();
			ret = true;
		}
		return ret;
	}

	typedef struct distance{
		float x;
		float y;
		float xy;
	};
	distance dist;

	int flag;
	int flag2;
	int center_distance;
	float rotate;



public:

	Mission_EtSumo(S32 timer = 0, S16 speed = ET_SPEED, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail;
	}

	virtual void Init(RobotInfo ri, NavInfo ni){
		tec_pid_.Init();
		tec_s_.Init(ri);
		tec_r_.Init();
		is_state_move_ = true;
		edge_ = Technic::LEFT;
		speed_ = ET_SPEED;
		local_status_ = eWAIT;
		//local_status_ = eRUN_CENTER;
		//local_status_ = eSTRAIGHT;
		flag = 0;
		flag2 = 0;
		preLock = 0;
		attackTimeS = 0;
		attackTimeR = 0;
		rotateTime = 0;
		org_pos_ = ni.pos;
		/*if (timer_ > 0) {
		TimerStart(timer_);
		}*/
		TimerStop();
		dist.x = ni.pos.X;
		dist.y = ni.pos.Y;
		dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
	}


	virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		bool ret = true;
		static int count = 0;
		static int count2 = 0;

		nowDistance = Robot::Instance().GetSonarDistance();
		//nowDistance = Robot::Instance().sonar_distance;

		switch (local_status_) {
		case eSTRAIGHT:
			speed_ = ET_SPEED;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			break;

		case eWAIT:             // 開始位置待ち
			speed_ = ET_SPEED;
			cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
			// 灰色のところまで進む
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > DIST_MARK){ // 灰色検知ができたらそれでやる
				local_status_ = eSTART;
			}
			break;

		case eSTART:            // 開始
			if (is_state_move_) {
				posBase1 = ni.pos;
				is_state_move_ = false;
			}
			// 90度だけ右回転する
			speed_ = 0;
			cmd = tec_r_.Calclate(speed_, -50, edge_);
			if( (posBase1.Ang - ni.pos.Ang) > 0.4 * M_PI ){
				is_state_move_ = true;
				local_status_ = eRUN_CENTER;
				flag = 0;
				Robot::Instance().Beep();
				tec_s_.Init(ri);
			}
			break;

		case eRUN_CENTER:       // 土俵中心へ移動
			if (is_state_move_) {
				flag2 = 0;
				posBase1 = ni.pos;
				is_state_move_ = false;
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
				dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
				switch(flag){
				case 0:
					center_distance  = DIST_CENTER1;
					break;
				case 2:
					center_distance  = DIST_CENTER2;
					break;
				case 3:
					center_distance  = DIST_CENTER3;
					break;
				}
			}
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			// ターゲット発見
			if(Robot::Instance().sonar_distance < 15){
				count++;
			}else {
				count = 0;
			}
			// ターゲット発見確定
			if(count > 3){
				speed_ = 0;
				is_state_move_ = true;
				local_status_ = eTURN;
				posGoal = ni.pos;
				count = 0;
				count2 = 0;
				Robot::Instance().Beep(Robot::A);
				rotateTime = 0;
				tec_r_.Init();
			}

			// 指定位置まで行ったら回転する
			if(((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > center_distance){
				speed_ = 0;
				is_state_move_ = true;
				count = 0;
				count2 = 0;
				local_status_ = eTURN;
				Robot::Instance().Beep(Robot::B);
				rotateTime = 0;
				tec_r_.Init();
			}
			else {
				speed_ = ET_SPEED;
			}
			break;

		case eTURN:             // 回転しペットボトルを探す
			if (is_state_move_) {
				is_state_move_ = false;
				posBase2 = ni.pos;
				preLock = 0;
				switch(flag){
				case 0:
					flag = 1;
					rotate = 1.9;
					posGoal = ni.pos;
					break;
				case 2:
					rotate = 1.45;
					posGoal = ni.pos;
					break;
				case 3:
					rotate = 2;
					posGoal = ni.pos;
					break;
				}
			}

			// 右回転で探す
			speed_ = 0;
			rotateTime++;
			cmd = tec_r_.Calclate(speed_, RO_SPEED, edge_);

			// ターゲット発見
			if(Robot::Instance().sonar_distance < TARGET_RANGE){
				count++;
			}else {
				count = 0;
			}
			// ターゲット発見確定
			if(count > 2){
				flag2 =1;
				Robot::Instance().Beep();
			}
			if(flag2 == 1 && nowDistance > forwardDistance){
				count2++;
			}else{
				//count2 = 0;
			}
			if(count2 > 3){
				is_state_move_ = true;
				local_status_ = eATTACK;
				count = 0;
				flag2 = 0;
				Robot::Instance().Beep(Robot::A);
			}
			if( (posBase2.Ang -ni.pos.Ang) > rotate * M_PI ){
				// 一回転して見つからなかった場合、あきらめる
				is_state_move_ = true;
				preLock = 0;
				//local_status_ = eGIVEUP;
				if(flag < 3){
					local_status_ = eRUN_CENTER;
					tec_s_.Init(ri);
				}else{
					local_status_ = eGIVEUP1;
					posBase1 = ni.pos;
					tec_r_.Init();
				}
				flag++;

			}
			break;

		case eATTACK:           // ET相撲実行中
			if (is_state_move_) {
				attackTimeR = 0;
				attackTimeS = 0;
				rotateTime = 0;
				TimerStart(20000);
				is_state_move_ = false;
			}
			else {
				//attackTime++;
			}

			if(nowDistance > 50){
				tec_r_.Init();
				speed_ = 0;
				rotateTime++;
				cmd = tec_r_.Calclate(0, -RO_SPEED, edge_);
			}else{
				speed_ = 40;
				//if( (nowDistance > forwardDistance) && (nowDistance < TARGET_RANGE) ){
				if( nowDistance > forwardDistance){
					tec_r_.Init();
					cmd = tec_r_.Calclate(speed_, 10, Technic::RIGHT);
					attackTimeR++;
					Robot::Instance().Beep();
				}
				else {
					tec_s_.Init(ri);
					cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
					attackTimeS++;
				}
			}
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
				// 黒ラインに到着したらET相撲実行を終了し、復帰へ
				is_state_move_ = true;
				local_status_ = eATTACK_COMPLETE;
				tec_r_.Init();
				Robot::Instance().Beep(Robot::B);
				Robot::Instance().Beep();
				attackTimeR = (int)((double)attackTimeR * 1.1);
				attackTimeS = (int)((double)attackTimeS * 1.1);
				rotateTime = (int)((double)rotateTime * 0.9);
			}

			if (IsTimeup()) {
				is_state_move_ = true;
				local_status_ = eATTACK_COMPLETE;
				tec_r_.Init();
				Robot::Instance().Beep(Robot::B);
				Robot::Instance().Beep();
				attackTimeR = (int)((double)attackTimeR * 1.1);
				attackTimeS = (int)((double)attackTimeS * 1.1);
				rotateTime = (int)((double)rotateTime * 0.9);
			}
			break;

		case eATTACK_COMPLETE:
			if(attackTimeS <= 0 && attackTimeR <= 0){
				// 土俵中心へ移動したときと同じ向きになるまで右回転する

				/*cmd = tec_r_.Calclate(speed_, 50, edge_);
				if( (ni.pos.Ang - posBase2.Ang) > 2.0 * M_PI ){
				local_status_ = eRECOVER1;
				Robot::Instance().Beep(Robot::A2);
				}*/
				if(rotateTime >=0){
				speed_ = 0;
				cmd = tec_r_.Calclate(speed_, RO_SPEED, edge_);
				Robot::Instance().Beep();
				rotateTime--;
				}else{
					local_status_ = eGOAL;
					is_state_move_ = true;
					preLock = 0;
					tec_r_.Init();
					Robot::Instance().Beep(Robot::A2);
				}

			}else {
				if(attackTimeR >= 0){
					attackTimeR--;
					speed_ = -40;
					cmd = tec_r_.Calclate(speed_, 10, edge_);
					Robot::Instance().Beep();
					if(attackTimeR <= 0){
						tec_s_.Init(ri);
					}

				}else{
					attackTimeS--;
					speed_ = -40;
					cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
					if(attackTimeS <= 0){
						tec_r_.Init();
					}
				}
			}
			break;
		case eGOAL:
			speed_ = 0;
			cmd = tec_r_.Calclate(speed_, RO_SPEED, edge_);

			if( flag == 0){
				rotate = 1.2;
			}else if( flag == 1){
				rotate = 1.2;
			}else if( flag == 2) {
				rotate = 1.2;
			}else if( flag == 3){
				rotate = 1.6;
			}
			if( (posGoal.Ang - ni.pos.Ang ) > rotate * M_PI ){
				is_state_move_ = true;
				local_status_ = eGIVEUP2;
				posBase1 = ni.pos;
				tec_s_.Init(ri);
			}
			break;

		case eGIVEUP1:
			/*// ET相撲をあきらめた場合はバックでレーンに復帰
			speed_ = -ET_SPEED;

			cmd = tec_s_.Calclate((S16)(speed_*0.5), ri.rot_encoderL_val, ri.rot_encoderR_val);
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
			is_state_move_ = true;
			local_status_ = eRECOVER1;
			posBase1 = ni.pos;
			Robot::Instance().Beep(Robot::A2);
			}*/
			// あきらめた場合は左前に進んでコース復帰
			speed_ = 0;
			cmd = tec_r_.Calclate(speed_, 40, edge_);
			if( (ni.pos.Ang - posBase1.Ang) > 0.4 * M_PI ){
				is_state_move_ = true;
				local_status_ = eGIVEUP2;
				tec_s_.Init(ri);
			}
			break;

		case eGIVEUP2:
			speed_ = ET_SPEED;
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
				flag = 100;
				dist.x = ni.pos.X;
				dist.y = ni.pos.Y;
				dist.xy = (dist.x * dist.x) + (dist.y * dist.y);
			}
				if( flag ==100 && ((ni.pos.X - dist.x) * (ni.pos.X - dist.x) +( ni.pos.Y - dist.y) * ( ni.pos.Y - dist.y)) > 10){
				// 黒ラインに到着したらET相撲実行を終了し、復帰へ
				is_state_move_ = true;
				local_status_ = eRECOVER1;
				tec_r_.Init();
				Robot::Instance().Beep(Robot::B);
				Robot::Instance().Beep();
			}
			break;

		case eRECOVER1:         // 方向転換
			// 90度ちょいだけ右回転する
			speed_ = 0;
			cmd = tec_r_.Calclate(speed_, -40, edge_);
			if( ( posBase1.Ang - ni.pos.Ang) > 0.5 * M_PI ){
				is_state_move_ = true;
				tec_r_.Init();
				local_status_ = eRECOVER2;
			}
			break;

		case eRECOVER2:         // 少し直進
			if (is_state_move_) {
				is_state_move_ = false;
				//TimerStart(5000);
				tec_s_.Init(ri);
			}
			speed_ = 10;

			cmd = tec_r_.Calclate(speed_, -5, edge_);
			//if(IsTimeup()){
			if( evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ){
				local_status_ = eRECOVER3;
				tec_pid_.Init();
				is_state_move_ = true;
				speed_ = 0;
				Robot::Instance().Beep(Robot::G);
			}
			break;

		case eRECOVER3:         // しばらくライントレースし、ミッション終了する
			if (is_state_move_) {
				tec_pid_.Init();
				is_state_move_ = false;
				//edge_ = Technic::RIGHT;
				TimerStart(5000);
			}

			cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, edge_);
			if(IsTimeup()){
				//local_status_ = eFINISH;
				Robot::Instance().Beep(Robot::F);
				speed_ = 50;
				ret = false;
			}
			break;

		default:
			ret = false;
			break;
		}

		forwardDistance = nowDistance;
		cmd.param3 = 0;

		return ret;

	}



private:

	TecPIDTrace    tec_pid_;
	TecStraight    tec_s_;
	TecRotate      tec_r_;
	Technic::DIRECTION edge_;
	S32 timer_;
	S16 speed_;
	S16 tail_;
	LocalState local_status_;
	bool is_state_move_;
	Position org_pos_;
	Position posBase1;
	Position posBase2;
	Position posGoal;
	S16 nowDistance;
	S16 forwardDistance;
	int preLock;
	int attackTimeS;
	int attackTimeR;
	int rotateTime;

	//    static Mission_EtSumo instance_;

};

//Mission_EtSumo Mission_EtSumo::instance_;
