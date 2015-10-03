
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

#include "jiro_type.h"
#include "MarkerCheck.h"

using namespace ecrobot;



class RobotObserver;



//************************************
// ロボットクラス
class Robot {
public:
	typedef RobotObserver* RobotObserverPtr;
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

	Bluetooth* getBT() const{ return &bluetooth_; }

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
        speaker_.playTone(tone, 100U, 20U);
    }
    void Beep(U32 freq, U32 duration, U32 volume){
        speaker_.playTone(freq, duration, volume);
    }

    // Bluetoothデータ出力
    void BTSend(RobotCmd, RobotInfo, NavInfo);

    // オブザーバ関係
    void AddObserver(RobotObserverPtr* obs, unsigned int size);

    // ロボットイベントID定義
    enum RobotEvent {
        EV_NONE = 0,
        EV_TOUCH_SENSOR_ON,             // タッチセンサONイベント
        EV_SONAR_SENSOR_ON,             // ソナーセンサONイベント
        EV_GYRO,                        // ジャイロが揺れたイベント
        EV_GYRO2,                        // ジャイロが揺れたイベント
        EV_GYRO3,                        // ジャイロが揺れたイベント
        EV_FAIL,                        // 転倒
        EV_LIGHT_HAZARD,                // 光センサハザード検知（灰色に使えへんかな）
        EV_LIGHT_BLACK,                 // 光センサ黒検知
        EV_MARKER,                      // マーカー検知
        EV_REMOTE_START,                // リモートスタートイベント
        EV_DUMMY_MAX                    // ダミー
    };

    VectorT<S8> calcPWM(VectorT<S16>);      // モータ出力値計算
	
	S16 sonar_distance;
	S16 gyro_data;
	S16 val_light;
	
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
	void ClearObservers();

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
    RobotObserverPtr* observer_list_;
	unsigned int observer_size_;


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
