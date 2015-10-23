
#include "utils.h"
#include "debug.h"
#include "Driver.h"
#include "Robot.h"
#include "Mission.h"
#include "CourseInfo.h"
#include "Navigator.h"
#include "Technic.h"

// BS一郎くん用ミッション定義
#ifdef ICHIRO
// インコース
# ifdef IN_COURSE
#  include "Mission_TailStraight.cpp"
# else
// アウトコース
#  include "Mission_TailStraight.cpp"
# endif

#else

// BS二郎くん用ミッション定義
// インコース
# ifdef IN_COURSE
#  include "Mission_TailStraight.cpp"
# else
// アウトコース
#  include "Mission_TailStraight.cpp"
# endif
#endif

// Driverタスクの起動周期[msec]
#define DRIVE_PERIOD       (4)

namespace
{
	/*struct RemoteMissionCreater{
		Mission* operator()(Bluetooth* pBT) const{
			return new Mission_Slope(  );
		}
	};*/
}

extern "C"
{
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

DeclareCounter(SysTimerCnt);
DeclareResource(Resource_DEList);
DeclareEvent(EventDrive);
DeclareAlarm(Alarm4msec);

//=============================================================================
// 関数名	: user_1ms_isr_type2
// 引数	: なし
// 戻り値	: なし
// 概要	: 1msec周期割り込みフック関数(OSEK ISR type2カテゴリ)
//=============================================================================
void user_1ms_isr_type2(void)
{
	(void)SignalCounter(SysTimerCnt); // Alarmハンドラ
	TecMusic::Instance().Player();
	SleeperMonitor(); // NxtおよびI2Cデバイスの使用時に必要
}

//*************************************************************************
// タスクエントリ
//*************************************************************************
TASK(TaskDrive){

	// コース情報設定
	
	Driver::MissionPtr ptr[] = {
		new Mission_TailStraight()
	};

	enum{ missoin_size = sizeof(ptr)/sizeof(ptr[0]) };

#ifdef IN_COURSE
	// インコース
	CourseInfo::type = CourseInfo::InCourse;
	Driver::Instance().AddMission(ptr, missoin_size);
#else
	// アウトコース
	CourseInfo::type = CourseInfo::OutCourse;
	Driver::Instance().AddMission(ptr, missoin_size);
#endif
	
    Driver::Instance().Process();

	 DEBUG_PRINT1("s", "Clear before");
	 Sleep_msec(2000);
	Driver::Instance().ClearMission();
    TerminateTask();
}

}


Driver* Driver::theInstance_ = 0;

//*************************************************************************
// コンストラクタ
//*************************************************************************
Driver::Driver() :
        status_(ST_BT_CONNECT),
		mission_list_(0),
		mission_size_(0),
		current_index_(0),
        robot_(Robot::Instance()),
        navigator_(Navigator::Instance()),
        is_bt_connect_(false)
{
	Robot::RobotObserverPtr ptr[] = {this};
    robot_.AddObserver(ptr,1);
    robot_.Stop();
}

//*************************************************************************
// デストラクタ
//*************************************************************************
Driver::~Driver(){
}

Driver& Driver::Instance(void){
    if ( !theInstance_ ){
        theInstance_ = new Driver();
    }
    return *(theInstance_);
}

//*************************************************************************
// ドライバクラスメインプロセス
//*************************************************************************
void Driver::Process(void){


    this->ClearEventFlag();
	state_self_introduction();
    this->ClearEventFlag();
    state_bt_connect();
    this->ClearEventFlag();
    state_calibration();
    this->ClearEventFlag();


    this->status_ = ST_WAIT_START;

    is_opened_ = true;

    VectorT<S16> cmd(50,0);

    while(is_opened_){
        switch(status_){
        case ST_WAIT_START:
            state_wait_start();
            break;
        case ST_DRIVING:
            state_driving();
            break;
        case ST_COMPLETE:
            DEBUG_PRINT1("s", "ST_COMPLETE");
            is_opened_ = false;
            break;
        case ST_ERROR:
            DEBUG_PRINT1("s", "ST_ERROR");
            is_opened_ = false;
            break;
        default:
            is_opened_ = false;
            break;
        }
    }

}


//*************************************************************************
// Mission追加
//*************************************************************************
void Driver::AddMission(MissionPtr* m, unsigned int size){
    if(mission_size_!=0)
	{
		ClearMission();
	}
	mission_list_ = new MissionPtr[size];
	for(unsigned int i = 0; i < size; ++i)
	{
		mission_list_[i] = m[i];
	}
	mission_size_ = size;
}

//*************************************************************************
// Missionクリア
//*************************************************************************
void Driver::ClearMission(){
	for(unsigned int i = 0; i < mission_size_; ++i)
	{
		delete mission_list_[i];
	}
    delete mission_list_;
	current_index_ = 0;
	mission_size_ = 0;
}



//*************************************************************************
// ロボットからのイベント通知受信
//*************************************************************************

void Driver::Notify(Robot::RobotEvent ev){
    this->SetEventFlag(ev);
}

//*************************************************************************
// 自己紹介処理
//*************************************************************************
SINT Driver::state_self_introduction(void){
    robot_.GetLCD().clear();

    robot_.GetLCD().cursor(0,0);
#ifdef ICHIRO
    robot_.GetLCD().putf("s", "Ichiro___!");
#else
    robot_.GetLCD().putf("s", "Jiro___!!");
#endif

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
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
		// Do nothing.
        Sleep_msec(10);
	}
	
    robot_.Beep();

    return 0;
}

//*************************************************************************
// Bluetoothコネクション処理
//*************************************************************************
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

    this->status_ = ST_CALIBRATION;
    return 0;
}

//*************************************************************************
// キャリブレーション処理
//*************************************************************************
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
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
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
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
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

    this->status_ = ST_WAIT_START;

    return 0;

}

#define TAIL_ANGLE_STAND_UP  106 /* 完全停止時の角度[度] */

//*************************************************************************
// スタート待ち処理
//*************************************************************************
SINT Driver::state_wait_start(void){

    RobotInfo ri;

    robot_.GetLCD().clear();
	robot_.GetLCD().putf("s", "TOUCH: Set Gyro Offset");
	robot_.GetLCD().disp();

    // タッチセンサ押下待ち
    this->ClearEventFlag();
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
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
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
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
	//RobotCmd cmd (0,0,0,RobotCmd::DIRECT_MODE);
    this->ClearEventFlag();
    while(1){
        if( (is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ||
            (is_SetEventFlag(Robot::EV_REMOTE_START)) ) {
            this->status_ = ST_DRIVING;
            robot_.GetLCD().putf("s", "Go!!");
            break;
        }
		robot_.Drive(cmd);
        Sleep_msec(10);
    }
    this->ClearEventFlag();
	
    return 0;
}


//*************************************************************************
// 運転中処理
//*************************************************************************
SINT Driver::state_driving(void){

    RobotCmd cmd;

	navigator_.Init();
	RobotInfo r_info = robot_.GetInfo();
	NavInfo n_info = navigator_.GetInfo(r_info);
    // 最初のミッションを設定＆初期化
    if ( current_index_ != mission_size_ )
	{
        mission_list_[current_index_]->Init(r_info, n_info);
    } else 
	{
        this->status_ = ST_ERROR;
    }

    this->ClearEventFlag();

    SetRelAlarm(Alarm4msec, 1, DRIVE_PERIOD); // ドライブ用Alarmタイマーのセット

    // 走行開始
    // ステータスが運転中ならループを繰り返す
    while(this->status_ == ST_DRIVING)
	{
        

        ClearEvent(EventDrive); // ドライブイベントのクリア
		WaitEvent(EventDrive);  // ドライブイベント待ち

        // タッチセンサON,もしくは転倒情報より走行停止
        if ( event_flag_.is_SetEvent(Robot::EV_TOUCH_SENSOR_ON) || event_flag_.is_SetEvent(Robot::EV_FAIL) ){
            status_ = ST_WAIT_START;
            break;
        }

        r_info = robot_.GetInfo();
        n_info = navigator_.GetInfo(r_info);

#if 1
        // Bluetooth データ送信
        if ( this->is_bt_connect_ )
		{
            robot_.BTSend(cmd, r_info, n_info);
        }
#endif
		int i=0;
        while(1)
		{
            bool ret = mission_list_[current_index_]->Run(r_info, n_info, this->event_flag_, cmd);
            if ( ret == true ){
                robot_.Drive(cmd);
                break;
            } 
			else 
			{
                // 次のミッションに遷移
                ++current_index_;
                if ( current_index_ == mission_size_ ){
                    // ミッションリストがなくなったので終了
                    this->status_ = ST_COMPLETE;
                    break;
                } else {
                    mission_list_[current_index_]->Init(r_info, n_info);
                    continue;
                }
            } 

			// Bluetooth データ送信
			if ( this->is_bt_connect_  && i==10)
			{
				robot_.BTSend(cmd, r_info, n_info);
				i=0;
			}
			i++;
			this->ClearEventFlag();
        }
    }

	DEBUG_PRINT1("s", "clear");
	Sleep_msec(2000);
    robot_.Stop();
    this->ClearEventFlag();
    CancelAlarm(Alarm4msec);            // ドライブ用Alarmタイマーのキャンセル
    ClearEvent(EventDrive); // ドライブイベントのクリア
    return 0;
}


//*************************************************************************
// イベントセット
//*************************************************************************
void Driver::SetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    this->event_flag_.Set(ev);
}


//*************************************************************************
// イベント確認
//*************************************************************************
bool Driver::is_SetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    return this->event_flag_.is_SetEvent(ev);
}

//*************************************************************************
// イベント確認
//*************************************************************************
void Driver::ClearEventFlag(void){
//    Lock l(Resource_DEList);
    this->event_flag_.Clear();
}
