#ifndef _INCLUDE_DRIVER_H_
#define _INCLUDE_DRIVER_H_


#include "Robot.h"
#include "Mission.h"
#include "Navigator.h"

class Driver : public RobotObserver {
public:
	typedef Mission* MissionPtr;

    // インスタンス取得
    static Driver& Instance(void);

    // メインプロセス
    void Process(void);

    //  Mission関係
    void AddMission(MissionPtr*, unsigned int size);

    //  Mission関係
    void ClearMission();

    //*************************************
    // RobotObserverI/F実装
    virtual void Notify(Robot::RobotEvent);


private:
    // タッチセンサ押下待ち
    void wait_touch_sensor(void);

    // 各ステータス毎の関数
	SINT state_self_introduction(void);
    SINT state_bt_connect(void);
    SINT state_calibration(void);
    SINT state_wait_start(void);
    SINT state_driving(void);


    void SetEventFlag(Robot::RobotEvent);
    bool is_SetEventFlag(Robot::RobotEvent);
    void ClearEventFlag(void);

    // ステータス
    enum DriverState {
        ST_BT_CONNECT = 0,
        ST_CALIBRATION,
        ST_WAIT_START,
        ST_DRIVING,
        ST_COMPLETE,
        ST_ERROR
    };

    DriverState status_;

    // ミッション関係
	MissionPtr* mission_list_;
	unsigned int mission_size_;
	unsigned int current_index_;

    // その他メンバ
    Robot& robot_;
    Navigator& navigator_;
    bool is_bt_connect_;
    bool is_opened_;

    // ロボットイベント
    EventFlag event_flag_;


    static Driver* theInstance_;
    Driver(void);
    ~Driver();


	//Mission

};





#endif
