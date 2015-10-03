#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

// メモ メモ
//　一郎
// 80～79　：　速度　100（直線からスタート）
// 89～88　：　速度　120（カーブ180°進行後実施）

//　二郎
// 74～75　：　速度　120　OK→こえすぎる
// 72～73　：　速度　120　OK→こえすぎる

// スタート時は左ライントレース
// 90度ターン後は右ライントレース


#define R2D(X) ((X) * M_PI/180.0) // ラジアンから度
#define D2R(X) ((X) * 180.0/M_PI) // 度からラジアン

//ジャイロオフセット変更用マクロ定義
#define SPEEDDOWN   -10 // 仮想後傾→現実前傾
#define SPEED_UP_   5   // 仮想前傾→現実後傾

//シーソ→階段変更時の角度補正マクロ定義
#define STAIRS_ST_LEN  1000.0 // 角度補正時の直線距離[mm]
#define STAIRS_DEF360  360    // シーソ終了後の補正値[°]
#define STAIRS_POINT   240    // 階段上り処理開始角度[°]

//階段上り処理繰り返し回数
#define STAIRS_CNT 2

// 階段速度
#define STAIRS_SPEED_ 110

//階段タイマ値
#define STAIRS_TIM1_1 1000 //衝突検知開始タイマ(1回目)
#define STAIRS_TIM1_2 400  //衝突検知開始タイマ(2回目以降)：二段目のはず
#define STAIRS_TIM3   800 //階段上り後処理時間


class Mission_Stairs : public Mission {
private:
    enum LocalState{
        STAIRS_IN = 0,     // 0_階段衝突前（角度補正～開始角度まで）
    	STAIRS_HIT,        // 1_階段モード１（階段衝突まで)  ｜
    	STAIRS_UP,         // 2_階段モード２（階段上り中）   ｜ここは繰り返し（３）
    	STAIRS_UP2,        // 3_階段モード３（階段上り後）   ｜
    	STAIRS_FAUL1,      // 4_階段モード４（階段上りきった直後）
    	STAIRS_FAUL2,      // 5_階段モード５（階段上りきってからターンまで）
		STAIRS_TURN,       // 6_階段モード６（90度ターン）
    	STAIRS_LOW,       // 7_階段モード７（落ちるまで直進）
		STAIRS_RECOVER,		// 8_階段モード８（ラインに復帰）
		STAIRS_LOW2,       // 9_階段モード９（ライントレース）
    	STAIRS_END,        // 10_階段処理終了
    };
public:

    Mission_Stairs(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        speed_ = STAIRS_SPEED_;
        local_status_ = STAIRS_IN;
    	//local_status_ = STAIRS_FAUL2;
    	//local_status_ = STAIRS_HIT;
    	tec_pid_.Init();
		is_state_move_ = true;
    	flag = 0;
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
    	switch(local_status_){
    	case STAIRS_IN :
			if (is_state_move_) {
				is_state_move_ = false;
				//tec_s_.Init(ri);
				tec_pid_.Init();
				//tec_s_.Init(ri);
				// シーソーイベント終了位置をスタート位置から360°と設定する。
				// →500mmで補正する。
		 		//Navigator::Instance().StartDirCorrection(STAIRS_ST_LEN,STAIRS_DEF360);
				timer_start(100);
			}
			cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);

    		//if( (ni.pos.Ang *180.0)/M_PI < STAIRS_POINT){
    		if(is_timeup()){
				local_status_ = STAIRS_HIT; //★→STAIRS_HIT
				is_state_move_ = true;
				Robot::Instance().Beep(Robot::B);
    		}
    		break;
    	case STAIRS_HIT :
			// 階段モード開始後、約１秒間PIDトレースを行う。
			// 1秒経過後、ジャイロ（７０）の揺れで次のステータスへ
			// ステータス3からの遷移の場合、タイマ値を変更して繰り返す。
			if (is_state_move_) {
				is_state_move_ = false;
				if(flag==0){// 1回目
					flag = 1;
					timer_start(STAIRS_TIM1_1);
				}else{      // 1回目以降
					if(flag < STAIRS_CNT){
						flag++;
						timer_start(STAIRS_TIM1_2);
					}else{
						is_state_move_ = true;
						local_status_ = STAIRS_FAUL1; //★→STAIRS_FAUL1
					}
				}
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		Robot::Instance().Beep();
			
    		//しっぽを上げる
    		cmd.param3 = 0;
    		
			if (is_timeup() && evf.is_SetEvent(Robot::EV_GYRO)) {
				is_state_move_ = true;
				local_status_ = STAIRS_UP; //★→2
				Robot::Instance().Beep(Robot::F);
			}
			break;
		case STAIRS_UP:
    		// ジャイロオフセットを‐10して仮想的に前傾とする。
    		// →階段を上ろうとするときに前傾と判断し加速しようとするのを軽減
    		
			if (is_state_move_) {
				is_state_move_ = false;
	    		Robot::Instance().SetGyro(-10);
				timer_start (STAIRS_TIM3);
				//tec_s_.Init(ri);
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		//cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);

    		if (is_timeup()) {
				local_status_ = STAIRS_UP2; //★→3
				Robot::Instance().Beep(Robot::D);
				is_state_move_ = true;
			}
    		break;
    	case STAIRS_UP2 :
    		// ここは前ステータスからの変更１回だけ通るので初期ステータス変更フラグ制御は不要
    		// 一定時間で前ステータスで変更したオフセット半分を戻す。
    		// →オフセットを変更すると、一定時間で加減速は終了する。
    		//   ただし、オフセットは変更したままなので倒立振子が不安定な状態になるため。
    		//   少しずつ戻さないとまた加減速が発生するため注意
    		Robot::Instance().SetGyro(5);
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		//cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			
    		local_status_ = STAIRS_HIT;
			break;
    	case STAIRS_FAUL1 :
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(100);
				Robot::Instance().ResetBalanceAPI();
				Navigator::Instance().ResetEncoder();
				tec_pid_.Init();
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, 80, Technic::LEFT);
    		//念のため、しっぽを上げる
    		cmd.param3 = 0;
    		
    		if (is_timeup()) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_FAUL2;
    		}    		
    		break;
    	case STAIRS_FAUL2 :		//上りきった
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(2000);
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, 80, Technic::LEFT);
    		cmd.param2 /= 5;
    		Robot::Instance().Beep(Robot::F);
    		if (is_timeup() && evf.is_SetEvent(Robot::EV_LIGHT_BLACK)) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_TURN;
    		}    		
    		break;
    		
    	case STAIRS_TURN :		// 90度回転
			if (is_state_move_) {
				is_state_move_ = false;
				posBase1 = ni.pos;
				tec_r_.Init();
			}
	    	speed_ = 0;
			cmd = tec_r_.Calclate(speed_, 40, Technic::LEFT);
			if( ( ni.pos.Ang - posBase1.Ang ) > 0.49 * M_PI ){
				is_state_move_ = true;
				local_status_ = STAIRS_LOW;
			}
    		break;
    	case STAIRS_LOW:		// 落ちるまで直進
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(500);
				speed_ = 100;
				tec_s_.Init(ri);
				//tec_pid_.Init();
			}
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
    			//cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, Technic::RIGHT);
    		if (is_timeup() && evf.is_SetEvent(Robot::EV_GYRO)) {
				is_state_move_ = true;
				local_status_ = STAIRS_RECOVER;
			}
    		break;
    	case STAIRS_RECOVER:		// 復帰処理をちゃんと入れたい
    		if (is_state_move_) {
				is_state_move_ = false;
				speed_ = 10;
    			timer_start(1000);
				tec_r_.Init();
			}
	    	cmd = tec_r_.Calclate(speed_, -5, Technic::RIGHT);
    			Robot::Instance().Beep(Robot::D);
			if ( is_timeup() && evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ) {
				is_state_move_ = true;
				local_status_ = STAIRS_LOW2;
			}
    		break;
    	case STAIRS_LOW2 :
			if (is_state_move_) {
				is_state_move_ = false;
				tec_pid_.Init();
				timer_start(3000);
			}
    		
    		cmd = tec_pid_.Calclate(ri.light_sensor_val, 10, Technic::RIGHT);
    		Robot::Instance().Beep(Robot::D);
    		if (is_timeup()) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_END;
    		}    		
    		break;
		case STAIRS_END:
    	default :
	    	ret = false;
    		break;
    	}
    	cmd.param3 = 0;
        return ret;
    }

    	virtual char* Name(void){
        return "Mission_Stairs";
    }

private:

    S16 speed_;
	S16 rotation_;
	F32 angSum;
	S32 angNum;
	F32 angBase;
	bool is_state_move_;
    TecStraight tec_s_;
	TecPIDTrace tec_pid_;
	TecRotate tec_r_;
    S8 local_status_;
    int            flag;
    Position posBase1;
};
