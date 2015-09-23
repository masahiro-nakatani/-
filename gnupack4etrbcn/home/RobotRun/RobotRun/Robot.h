
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
// ���{�b�g�N���X
class Robot {
public:
	typedef RobotObserver* RobotObserverPtr;
    static Robot& Instance(void);

    // ���C���v���Z�X
    void Process(void);

    // Bluetooth�R�l�N�g�҂��w��
    SINT BTConnect(void);

    // �W���C���Z���T�L�����u���[�V�����w��
    void Calibration(void);

    // ���[�^�o�͎w��
    void Drive(RobotCmd);

	// �����ې���
	void TailControl(RobotCmd&);

    // ��~�w��
    void Stop(void);
	
	// �|��API���Z�b�g
	void ResetBalanceAPI(void);

    // ���{�b�g���擾
    RobotInfo GetInfo(void);
	
	S16 GetGyroOffset();
	
	void SetGyroOffset(S16);

    // LCD�I�u�W�F�N�g�擾
    Lcd& GetLCD(void){
        return lcd_;
    }

	// �\�i�[�����擾
	S16 GetSonarDistance(void){
		return sonar_distance;
	}

	Bluetooth* getBT() const{ return &bluetooth_; }

	enum RobotTone {
		C = 261, // �h
		D = 293, // ��
		E = 329, // �~
		F = 349, // �t�@
		G = 392, // �\.
		A = 440, // ��
		B = 493, // �V
		A2 = 880, // ��+
	};

    // �r�[�v��
    void Beep(S16 tone = A){
        speaker_.playTone(tone, 100U, 20U);
    }
    void Beep(U32 freq, U32 duration, U32 volume){
        speaker_.playTone(freq, duration, volume);
    }

    // Bluetooth�f�[�^�o��
    void BTSend(RobotCmd, RobotInfo, NavInfo);

    // �I�u�U�[�o�֌W
    void AddObserver(RobotObserverPtr* obs, unsigned int size);

    // ���{�b�g�C�x���gID��`
    enum RobotEvent {
        EV_NONE = 0,
        EV_TOUCH_SENSOR_ON,             // �^�b�`�Z���TON�C�x���g
        EV_SONAR_SENSOR_ON,             // �\�i�[�Z���TON�C�x���g
        EV_GYRO,                        // �W���C�����h�ꂽ�C�x���g
        EV_GYRO2,                        // �W���C�����h�ꂽ�C�x���g
        EV_GYRO3,                        // �W���C�����h�ꂽ�C�x���g
        EV_FAIL,                        // �]�|
        EV_LIGHT_HAZARD,                // ���Z���T�n�U�[�h���m�i�D�F�Ɏg���ւ񂩂ȁj
        EV_LIGHT_BLACK,                 // ���Z���T�����m
        EV_MARKER,                      // �}�[�J�[���m
        EV_REMOTE_START,                // �����[�g�X�^�[�g�C�x���g
        EV_DUMMY_MAX                    // �_�~�[
    };

    VectorT<S8> calcPWM(VectorT<S16>);      // ���[�^�o�͒l�v�Z
	
	S16 sonar_distance;
	S16 gyro_data;
	S16 val_light;
	
	void GetMotorEncoder(S32 &lmotor,S32 &rmotor);
	S16 GetGyro(void); // ����̓_��
	S16 SetGyro(S16 up);//�@�������W���C���I�t�Z�b�g�����Z
private:
    Robot(void);
    ~Robot(void);


    void CheckTouchSensor(void);        // �^�b�`�Z���T��ԃ`�F�b�N
    void CheckSonarSensor(void);        // �\�i�[�Z���T��ԃ`�F�b�N
    void CheckGyroSensor(void);         // �W���C���Z���T��ԃ`�F�b�N
    void CheckLightSensor(void);        // ���Z���T�`�F�b�N
    void CheckRemoteStart(void);        // Bluetooth�����[�g�X�^�[�g�`�F�b�N

    void CallNotify(RobotEvent);        // �C�x���g�ʒm
	void ClearObservers();

    //***************************
    // �f�o�C�X�n�����o
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
    // ���̑������o
    //**************************

    S16 gyro_offset_;
    S16 tail_angle_;
    bool touch_sensor_state_;
    bool sonar_sensor_state_;
    SINT bt_connect_state_;
	MarkerCheck& marker_check_;

    // �I�u�U�[�o���X�g
    RobotObserverPtr* observer_list_;
	unsigned int observer_size_;


    //***************************
    //�V���O���g���C���X�^���X
    //***************************
    static Robot* theInstance_;

};


//************************************
// ���{�b�g�I�u�U�[�o�N���X
class RobotObserver {
public:
    RobotObserver(){}
    virtual ~RobotObserver(){}
    virtual void Notify(Robot::RobotEvent){}
};




#endif
