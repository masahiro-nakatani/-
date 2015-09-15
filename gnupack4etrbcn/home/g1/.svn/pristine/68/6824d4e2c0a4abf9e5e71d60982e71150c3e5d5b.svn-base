#ifndef _INCLUDE_DRIVER_H_
#define _INCLUDE_DRIVER_H_

/**
 * @file Driver.h
 * @brief Driverクラスの定義
 */

#include "Robot.h"
#include "List.h"
#include "Mission.h"
#include "Navigator.h"

using namespace nro;

/**
 * コースに応じた Mission を保持し、
 * Mission の遷移や、 Robot の制御 を行う。
 *
 * @note Singleton パターン。
 */
class Driver : public RobotObserver {
public:

    /** Driverのインスタンスを取得する。 */
    static Driver& Instance(void);

    /** ドライバクラスメインプロセス */
    void Process(void);

    /** Mission を追加する */
    void AddMission(Mission*);

    /** 登録されているすべての Mission をキューから取り除く */
    void ClearMission();

    /** Robot からのイベント通知を受信する。*/    
    virtual void Notify(Robot::RobotEvent);

private:
    /** タッチセンサ押下待ち */
    void wait_touch_sensor(void); //!< @todo 定義がなく、どこからも呼び出されていない。

    // 各ステータス毎の関数

    /** LCDで自己紹介する */
	SINT state_self_introduction(void);

    /** Bluetoothの接続を行う */
    SINT state_bt_connect(void);

    /** センサのキャリブレーションを行う */
    SINT state_calibration(void);

    /** スタート待ち */
    SINT state_wait_start(void);

    /** 走行中の処理 */
    SINT state_driving(void);

    /**
     * イベントフラグをセットする
     * @param ev セット状態にするイベント
     */
    void SetEventFlag(Robot::RobotEvent);
    bool IsSetEventFlag(Robot::RobotEvent);
    void ClearEventFlag(void);

    /** ステータス */
    enum DriverState {
        STATE_BT_CONNECT = 0,
        STATE_CALIBRATION,
        STATE_WAIT_START,
        STATE_DRIVING,
        STATE_COMPLETE,
        STATE_ERROR
    };

    DriverState status_;

    // ミッション関係
    List<Mission*> mission_list_; //!< 走行する各ミッション
    List<Mission*>::Iterator cur_mission_iter_; //!< mission_list_のイテレータ @todo 参照範囲が特定の関数なのでスコープを狭くする

    Robot& robot_; //!< Driver が操作する Robot
    Navigator& navigator_;
    bool is_bt_connect_; //!< Bluetoothが接続状態ならばtrue、未接続ならfals
    bool is_opened_; //!< 走行中ならtrue。 @todo 参照範囲が特定の関数なのでスコープを狭くする

    EventFlag event_flag_; //!< ロボットイベント

    static Driver* theInstance_; //!< Singleton のため、自身を保持する。
       
    Driver(void);
    ~Driver();

};

#endif
