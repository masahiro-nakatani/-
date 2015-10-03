#ifndef _INCLUDE_DRIVER_H_
#define _INCLUDE_DRIVER_H_


#include "Robot.h"
#include "Mission.h"
#include "Navigator.h"

class Driver : public RobotObserver {
public:
	typedef Mission* MissionPtr;

    // �C���X�^���X�擾
    static Driver& Instance(void);

    // ���C���v���Z�X
    void Process(void);

    //  Mission�֌W
    void AddMission(MissionPtr*, unsigned int size);

    //  Mission�֌W
    void ClearMission();

    //*************************************
    // RobotObserverI/F����
    virtual void Notify(Robot::RobotEvent);


private:
    // �^�b�`�Z���T�����҂�
    void wait_touch_sensor(void);

    // �e�X�e�[�^�X���̊֐�
	SINT state_self_introduction(void);
    SINT state_bt_connect(void);
    SINT state_calibration(void);
    SINT state_wait_start(void);
    SINT state_driving(void);


    void SetEventFlag(Robot::RobotEvent);
    bool is_SetEventFlag(Robot::RobotEvent);
    void ClearEventFlag(void);

    // �X�e�[�^�X
    enum DriverState {
        ST_BT_CONNECT = 0,
        ST_CALIBRATION,
        ST_WAIT_START,
        ST_DRIVING,
        ST_COMPLETE,
        ST_ERROR
    };

    DriverState status_;

    // �~�b�V�����֌W
	MissionPtr* mission_list_;
	unsigned int mission_size_;
	unsigned int current_index_;

    // ���̑������o
    Robot& robot_;
    Navigator& navigator_;
    bool is_bt_connect_;
    bool is_opened_;

    // ���{�b�g�C�x���g
    EventFlag event_flag_;


    static Driver* theInstance_;
    Driver(void);
    ~Driver();


	//Mission

};





#endif
