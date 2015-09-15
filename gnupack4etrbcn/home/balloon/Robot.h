
#ifndef _INCLUDE_ROBOT_H_
#define _INCLUDE_ROBOT_H_

#include "TouchSensor.h"
#include "SonarSensor.h"
#include "LightSensor.h"
#include "GyroSensor.h"
#include "Motor.h"
#include "Nxt.h"
#include "Lcd.h"
#include "Speaker.h"
#include "Daq.h"
#include "Bluetooth.h"
#include "BTConnection.h"

#include "Vector.h"
#include "List.h"

#include "InfoType.h"
#include "MarkerCheck.h"

using namespace ecrobot;



class RobotObserver;



//************************************
// ロボットクラス
class Robot {
public:
    static Robot& Instance(void);

    // メインプロセス
    void Process(void);

    // Bluetoothコネクト待ち指示
    SINT BTConnect(void);

    // ジャイロセンサキャリブレーション指示
    void Calibration(void);

    // モータ出力指示
    void Drive(RobotCmd);

	// しっぽ制御
	void TailControl(RobotCmd&);

    // 停止指示
    void Stop(void);

	// 倒立APIリセット
	void ResetBalanceAPI(void);

    // ロボット情報取得
    RobotInfo GetInfo(void);

	S16 GetGyroOffset();

	void SetGyroOffset(S16);

    // LCDオブジェクト取得
    Lcd& GetLCD(void){
        return lcd_;
    }

	// ソナー距離取得
	S16 GetSonarDistance(void){
		return sonar_distance;
	}

	enum RobotTone {
		C = 261, // ド
		D = 293, // レ
		E = 329, // ミ
		F = 349, // ファ
		G = 392, // ソ.
		A = 440, // ラ
		B = 493, // シ
		A2 = 880, // ラ+
	};

    // ビープ鳴動
    void Beep(S16 tone = A){
        speaker_.playTone(tone, 100U, 80U);
    }
    void Beep(U32 freq, U32 duration, U32 volume){
        speaker_.playTone(freq, duration, volume);
    }

    // Bluetoothデータ出力
    void BTSend(RobotCmd, RobotInfo, NavInfo);
    //void BTSend(RobotCmd, Position);

    // オブザーバ関係
    void AddObserver(RobotObserver* obs);
    void RemoveObserver(RobotObserver* obs);

    // ロボットイベントID定義
    enum RobotEvent {
        EVENT_NONE = 0,
        EVENT_TOUCH_SENSOR_ON,             // タッチセンサONイベント
        EVENT_SONAR_SENSOR_ON,             // ソナーセンサONイベント
        EVENT_GYRO,                        // ジャイロが揺れたイベント
        EVENT_GYRO2,                        // ジャイロが揺れたイベント
        EVENT_GYRO3,                        // ジャイロが揺れたイベント
        EVENT_FAIL,                        // 転倒
        EVENT_LIGHT_HAZARD,                // 光センサハザード検知（灰色に使えへんかな）
        EVENT_LIGHT_BLACK,                 // 光センサ黒検知
        EVENT_MARKER,                      // マーカー検知
        EVENT_REMOTE_START,                // リモートスタートイベント
        EVENT_DUMMY_MAX                    // ダミー
    };

    VectorT<S8> calcPWM(VectorT<S16>);      // モータ出力値計算

	S16 sonar_distance;
	S16 gyro_data;
	S16 val_light;

    // ロボちゃんの状態(ログ出力用)
    // usage
    //   Robot::Instance().current_state = Robot::SEESAW_S_DOWN;
    enum RobotState4Log{
        None = 0, //初期状態

        // シーソーの各状態
		SEESAW_P_REACHING,
        SEESAW_S_APPROACH_BACK,
		SEESAW_S_STAGING,
        SEESAW_P_RECOVER_ON_SEESAW,
		SEESAW_S_DOWN,
		SEESAW_S_STAY_ON_SEESAW,
		SEESAW_S_BACK_ON_SEESAW,
        SEESAW_S_GET_OFF,
        SEESAW_R_FACE_TO_LINE,
		SEESAW_S_RECOVER_ON_GROUND,
        SEESAW_P_STABLE_SHORT,
		SEESAW_P_FOR_NEXT_MISSION,
        SEESAW_END,
    };
    S32 current_state; //現在の状態。

	void GetMotorEncoder(S32 &lmotor,S32 &rmotor);
	S16 GetGyro(void); // これはダメ
	S16 SetGyro(S16 up);//　引数分ジャイロオフセットを加算
private:
    Robot(void);
    ~Robot(void);


    void CheckTouchSensor(void);        // タッチセンサ状態チェック
    void CheckSonarSensor(void);        // ソナーセンサ状態チェック
    void CheckGyroSensor(void);         // ジャイロセンサ状態チェック
    void CheckLightSensor(void);        // 光センサチェック
    void CheckRemoteStart(void);        // Bluetoothリモートスタートチェック

    void CallNotify(RobotEvent);        // イベント通知


    //***************************
    // デバイス系メンバ
    //***************************
    TouchSensor& touch_sensor_;
    SonarSensor& sonar_sensor_;
    LightSensor& light_sensor_;
    GyroSensor& gyro_sensor_;
    Motor& motorL_;
    Motor& motorR_;
	Motor& tail_;
    Bluetooth& bluetooth_;
    Lcd& lcd_;
    Nxt& nxt_;
    Speaker& speaker_;
    Daq daq_;

    //**************************
    // その他メンバ
    //**************************

    S16 gyro_offset_;
    S16 tail_angle_;
    bool touch_sensor_state_;
    bool sonar_sensor_state_;
    SINT bt_connect_state_;
	MarkerCheck& marker_check_;

    // オブザーバリスト
    nro::List<RobotObserver*> observer_list_;


    //***************************
    //シングルトンインスタンス
    //***************************
    static Robot* theInstance_;
};


//************************************
// ロボットオブザーバクラス
class RobotObserver {
public:
    RobotObserver(){}
    virtual ~RobotObserver(){}
    virtual void Notify(Robot::RobotEvent){}
};




#endif
