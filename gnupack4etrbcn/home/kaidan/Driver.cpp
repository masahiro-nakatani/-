
/**
 * @file Driver.cpp
 * @brief Driverクラスの実装
 */

#include "Utils.h"
#include "Debug.h"
#include "Driver.h"
#include "Robot.h"
#include "Mission.h"
#include "CourseInfo.h"
#include "Navigator.h"
#include "Technic.h"

// コースによって分岐。
// ロボちゃんの容量が少ないので、不要なファイルはビルドしない
//#include "MissionDefault.cpp"
//#include "MissionStart.cpp"
//#include "MissionSlope.cpp"
#include "MissionKaidan.cpp"
#include "MissionSound.cpp"
#ifdef IN_COURSE
  #  include "Mission_S_Turn.cpp"
//  #include "MissionLookupGateDouble.cpp"

#else // OUT_COURSE
  #  include "Mission_S_Turn.cpp"
  /* 安定のSingleか、冒険のDoubleか。 */
//#include "MissionSeesawSingle.cpp"
//  #include "MissionSeesawDouble.cpp"
#endif
//#include "MissionGarageIn.cpp"

#define DRIVE_PERIOD       (4) //<! Driverタスクの起動周期[msec]

extern "C"
{
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

DeclareCounter(SysTimerCnt);
DeclareResource(Resource_DEList);
DeclareEvent(EventDrive);
DeclareAlarm(Alarm4msec);

/**
 * 1msec周期割り込みフック関数(OSEK ISR type2カテゴリ)
 */
void user_1ms_isr_type2(void)
{
	(void)SignalCounter(SysTimerCnt); // Alarmハンドラ
	TecMusic::Instance().Player();
	SleeperMonitor(); // NxtおよびI2Cデバイスの使用時に必要
}

/**
 * Driverタスクのエントリポイント
 *
 * コースに応じた Mission を追加して、走行する。
 */
TASK(TaskDrive){

	// コース情報設定
#ifdef IN_COURSE
	// インコース
	CourseInfo::type = CourseInfo::InCourse;
	Driver::Instance().AddMission(new MissionKaidan());
	//Driver::Instance().AddMission(new Mission_S_Turn());
//	Driver::Instance().AddMission(new Mission_Slope(Technic::RIGHT));
//	Driver::Instance().AddMission(new Mission_LookupGate());
#else // OUT_COURSE
	CourseInfo::type = CourseInfo::OutCourse;
	Driver::Instance().AddMission(new MissionKaidan());
//	Driver::Instance().AddMission(new MissionStart());
//	Driver::Instance().AddMission(new Mission_Slope());
//	Driver::Instance().AddMission(new Mission_Seesaw());
#endif
//	Driver::Instance().AddMission(new Mission_GarageIn());

    Driver::Instance().Process();
	Driver::Instance().ClearMission();
    TerminateTask();

} // end of TASK(TaskDrive)
} // end of extern "C"


Driver* Driver::theInstance_ = 0; //< 自身の参照。Singleton用

/**
 * 初期状態でインスタンス生成する。
 * DriverはSingletonなので、インスタンスの生成は Driver::Instance で行う。
 */
Driver::Driver() :
        status_(STATE_BT_CONNECT),
        robot_(Robot::Instance()),
        navigator_(Navigator::Instance()),
        is_bt_connect_(false)
{
    robot_.AddObserver(this);
    robot_.Stop();
}

/** 
 * デストラクタ
 */
Driver::~Driver(){
    //明示的に破棄するものはない。
}

/**
 * Driverのインスタンスを取得する。
 *
 * Driver::Instance(void) の初回呼び出し時にDriverインスタンスを生成する。
 * 以降の Driver::Instance(void) の呼び出しにおいては、その参照を返す。
 */
Driver& Driver::Instance(void){
    if ( !theInstance_ ){
        theInstance_ = new Driver();
    }
    return *(theInstance_);
}

/**
 * ドライバクラスメインプロセス
 * 
 * ロボットの初期化から終了までの処理を行う。
 */
void Driver::Process(void){

    this->ClearEventFlag();
	state_self_introduction();
    this->ClearEventFlag();
    state_bt_connect();
    this->ClearEventFlag();
    state_calibration();
    this->ClearEventFlag();

    this->status_ = STATE_WAIT_START;

    is_opened_ = true;

    VectorT<S16> cmd(50,0);

    while(is_opened_){
        switch(status_){
        case STATE_WAIT_START:
            state_wait_start();
            break;
        case STATE_DRIVING:
            state_driving();
            break;
        case STATE_COMPLETE:
            DEBUG_PRINT1("s", "STATE_COMPLETE");
            is_opened_ = false;
            break;
        case STATE_ERROR:
            DEBUG_PRINT1("s", "STATE_ERROR");
            is_opened_ = false;
            break;
        default:
            is_opened_ = false;
            break;
        }
    }
}


/** 
 *  Mission を追加する
 * @param m 追加するミッション
 */
void Driver::AddMission(Mission* m){
    this->mission_list_.PushBack(m);
}

/**
 * 登録されているすべての Mission をキューから取り除く
 */
void Driver::ClearMission(){
    this->mission_list_.Clear();
}

/**
 * Robot からのイベント通知を受信する。
 * @brief この実装では、evに対応するイベントフラグを設定する。
 * @param ev イベント
 */
void Driver::Notify(Robot::RobotEvent ev){
    this->SetEventFlag(ev);
}

/**
 * タッチセンサが押下されるまで、LCD に プログラム名、In/Outコースを表示する。
 * @return 常に0を返す。
 */
SINT Driver::state_self_introduction(void){
    robot_.GetLCD().clear();

    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "N-Ro!");

    robot_.GetLCD().cursor(2,1);
#ifdef IN_COURSE
    robot_.GetLCD().putf("s", "In Course Mode");
#else
    robot_.GetLCD().putf("s", "Out Course Mode");
#endif

    robot_.GetLCD().cursor(0,4);
    robot_.GetLCD().putf("s", "Push Touch Sensor!");
    robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
		// Do nothing.
        Sleep_msec(10);
	}

    robot_.Beep();

    return 0;
}


/** 
 * Bluetoothの接続を行う 
 * @return 常に0を返す。
*/
SINT Driver::state_bt_connect(void){
    SINT ret;

    ret = robot_.BTConnect();

    switch (ret){
    case 1: //コネクト成功
        this->is_bt_connect_ = true;
        break;
    default:
        this->is_bt_connect_ = false;
    }

    this->status_ = STATE_CALIBRATION;

    return 0;
}

/** 
 * 輝度値センサのキャリブレーションを行う。
 * @return 常に0を返す。
 */
SINT Driver::state_calibration(void){

    //******************
    // 黒色値初期化
    robot_.GetLCD().clear();
    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "Input Black");
    robot_.GetLCD().disp();

    RobotInfo ri;
    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
        robot_.GetLCD().cursor(1, 2);
        ri = robot_.GetInfo();
        robot_.GetLCD().putf("sd", "Set: ", ri.light_sensor_val, 0);
        robot_.GetLCD().disp();
        Sleep_msec(10);
    }

    ri = robot_.GetInfo();
    CourseInfo::colorBlack = ri.light_sensor_val;

    Sleep_msec(500);
    robot_.Beep();

    //******************
    // 白色値初期化
    robot_.GetLCD().clear();
    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "Input White");
    robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
        robot_.GetLCD().cursor(1, 2);
        ri = robot_.GetInfo();
        robot_.GetLCD().putf("sd", "Set: ", ri.light_sensor_val, 0);
        robot_.GetLCD().disp();
        Sleep_msec(100);
    }

    ri = robot_.GetInfo();
    CourseInfo::colorWhite = ri.light_sensor_val;

    Sleep_msec(500);
    robot_.Beep();

   //******************
    // 白色値初期化（２）
    robot_.GetLCD().clear();
    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "Input wood");
    robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
        robot_.GetLCD().cursor(1, 2);
        ri = robot_.GetInfo();
        robot_.GetLCD().putf("sd", "Set: ", ri.light_sensor_val, 0);
        robot_.GetLCD().disp();
        Sleep_msec(100);
    }

    ri = robot_.GetInfo();
    CourseInfo::colorGray = ri.light_sensor_val;

    Sleep_msec(500);
    robot_.Beep();


    this->status_ = STATE_WAIT_START;

    return 0;

}

#define TAIL_ANGLE_STAND_UP  106 /* 完全停止時の角度[度] */


/** 
 * ジャイロセンサのキャリブレーションとしっぽ位置の調整を行う。
 * @return 常に0を返す。
 */
SINT Driver::state_wait_start(void){

    RobotInfo ri;

 	//robot_.GetLCD().clear();
	//robot_.GetLCD().putf("s", "TOUCH: Set Gyro Offset");
	//robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
        robot_.GetLCD().cursor(1, 2);
        ri = robot_.GetInfo();
        robot_.GetLCD().putf("sd", "Set: ", ri.gyro_sensor_val, 0);
        robot_.GetLCD().disp();
        Sleep_msec(100);
    }
    //ジャイロキャリブレーション
    robot_.Calibration();
    Sleep_msec(500);
    robot_.Beep();

	robot_.GetLCD().clear();
	robot_.GetLCD().putf("s", "TOUCH: Reset Tail");
	robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ){
        Sleep_msec(100);
    }
	// しっぽクリア
	robot_.Stop();
    Sleep_msec(500);
    robot_.Beep();

	robot_.GetLCD().clear();
	robot_.GetLCD().putf("s", "TOUCH:START/STOP");
	robot_.GetLCD().disp();

	RobotCmd cmd (0,0,TAIL_ANGLE_STAND_UP,RobotCmd::DIRECT_MODE);
    this->ClearEventFlag();
    while(1){
        if( (IsSetEventFlag(Robot::EVENT_TOUCH_SENSOR_ON)) ||
            (IsSetEventFlag(Robot::EVENT_REMOTE_START)) ) {
            this->status_ = STATE_DRIVING;
            robot_.GetLCD().putf("s", "Go!!");
            break;
        }
		robot_.Drive(cmd);
        Sleep_msec(10);
    }
    this->ClearEventFlag();

    return 0;
}


/** 走行中の処理 */
SINT Driver::state_driving(void){

    //DEBUG_PRINT1("s", "state_driving()");

    RobotInfo r_info;
    NavInfo  n_info;

    RobotCmd cmd;

    bool ret;

	navigator_.Init();

	//Position prePos;
	//Position now_position;
 //   F32 r = 41.34;    // タイヤの半径 []
 //   F32 d = 82.6;      // 左右のタイヤ間距離 [mm]
 //   S32 angR = nxt_motor_get_count(1);
 //   S32 angL = nxt_motor_get_count(2);
 //   S32 angR_bak;
 //   S32 angL_bak;


	navigator_.Init();
    r_info = robot_.GetInfo();
    //angR = r_info.rot_encoderR_val;
    //angL = r_info.rot_encoderL_val;
	n_info = navigator_.GetInfo(r_info);
    // 最初のミッションを設定＆初期化
    cur_mission_iter_ = this->mission_list_.Begin();
    if ( cur_mission_iter_ != this->mission_list_.End() ){
        (*cur_mission_iter_)->Init(r_info, n_info);
    } else {
        this->status_ = STATE_ERROR;
    }

    this->ClearEventFlag();

    SetRelAlarm(Alarm4msec, 1, DRIVE_PERIOD); // ドライブ用Alarmタイマーのセット

    // 走行開始
    // ステータスが運転中ならループを繰り返す
    while(this->status_ == STATE_DRIVING){
        //DEBUG_PRINT1("s", (*cur_mission_iter_)->Name());
        ClearEvent(EventDrive); // ドライブイベントのクリア
		WaitEvent(EventDrive);  // ドライブイベント待ち

        // タッチセンサON,もしくは転倒情報より走行停止
        if ( event_flag_.is_SetEvent(Robot::EVENT_TOUCH_SENSOR_ON) || event_flag_.is_SetEvent(Robot::EVENT_FAIL) ){
            status_ = STATE_WAIT_START;
            break;
        }

        // デバッグ用
        if ( event_flag_.is_SetEvent(Robot::EVENT_GYRO) ){
//            robot_.Beep();
        }
        if ( event_flag_.is_SetEvent(Robot::EVENT_GYRO2) ){
//            robot_.Beep();
        }

        if ( event_flag_.is_SetEvent(Robot::EVENT_SONAR_SENSOR_ON) ){
//            robot_.Beep();
        }

        r_info = robot_.GetInfo();
        n_info = navigator_.GetInfo(r_info);


        //prePos = now_position;
        //angR_bak = angR;
        //angL_bak = angL;
        //angR = r_info.rot_encoderR_val;

        //angL = r_info.rot_encoderL_val;
        //now_position = getPositionEstimation(prePos, angR - angR_bak, angL - angL_bak, r, d);

        while(1){
            ret = (*cur_mission_iter_)->Run(r_info, n_info, this->event_flag_, cmd);
            if ( ret == true ){
                robot_.Drive(cmd);
                break;
            } else if ( ret == false ){
                // 次のミッションに遷移
                ++cur_mission_iter_;
                if ( cur_mission_iter_ == mission_list_.End() ){
                    // ミッションリストがなくなったので終了
                    // ただし、最後のミッションは倒立したまま停止を続けるはずなので、
                    // 基本的にこの処理には入らない
                    this->status_ = STATE_COMPLETE;
                    break;
                } else {
                    (*cur_mission_iter_)->Init(r_info, n_info);
                    continue;
                }
            } else {
                // ここに入ることはないはず
                this->status_ = STATE_WAIT_START;
                break;
            }
        }

#if 0
        // Bluetooth データ送信
        if ( this->is_bt_connect_ ){
            robot_.BTSend(cmd, r_info, n_info);
            //robot_.BTSend(cmd, now_position);
        }
#endif

        this->ClearEventFlag();
    }

    robot_.Stop();
    this->ClearEventFlag();
    CancelAlarm(Alarm4msec);            // ドライブ用Alarmタイマーのキャンセル
    ClearEvent(EventDrive); // ドライブイベントのクリア
    return 0;
}


/**
 * イベントフラグをセットする
 * @param ev セット状態にするイベント
 */
void Driver::SetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    this->event_flag_.Set(ev);
}


/**
 * イベントフラグの設定状態を取得する
 * @param ev イベント
 * @retval TRUE evのがイベントフラグが設定されている
 * @retval FALSE evのがイベントフラグが設定されていない
 */
bool Driver::IsSetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    return this->event_flag_.is_SetEvent(ev);
}


/** 
 * すべてのイベントフラグをクリアする
 */
void Driver::ClearEventFlag(void){
//    Lock l(Resource_DEList);
    this->event_flag_.Clear();
}





