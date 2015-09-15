#ifndef _INCLUDE_DRIVER_H_
#define _INCLUDE_DRIVER_H_

/**
 * @file Driver.h
 * @brief Driver�N���X�̒�`
 */

#include "Robot.h"
#include "List.h"
#include "Mission.h"
#include "Navigator.h"

using namespace nro;

/**
 * �R�[�X�ɉ����� Mission ��ێ����A
 * Mission �̑J�ڂ�A Robot �̐��� ���s���B
 *
 * @note Singleton �p�^�[���B
 */
class Driver : public RobotObserver {
public:

    /** Driver�̃C���X�^���X���擾����B */
    static Driver& Instance(void);

    /** �h���C�o�N���X���C���v���Z�X */
    void Process(void);

    /** Mission ��ǉ����� */
    void AddMission(Mission*);

    /** �o�^����Ă��邷�ׂĂ� Mission ���L���[�����菜�� */
    void ClearMission();

    /** Robot ����̃C�x���g�ʒm����M����B*/    
    virtual void Notify(Robot::RobotEvent);

private:
    /** �^�b�`�Z���T�����҂� */
    void wait_touch_sensor(void); //!< @todo ��`���Ȃ��A�ǂ�������Ăяo����Ă��Ȃ��B

    // �e�X�e�[�^�X���̊֐�

    /** LCD�Ŏ��ȏЉ�� */
	SINT state_self_introduction(void);

    /** Bluetooth�̐ڑ����s�� */
    SINT state_bt_connect(void);

    /** �Z���T�̃L�����u���[�V�������s�� */
    SINT state_calibration(void);

    /** �X�^�[�g�҂� */
    SINT state_wait_start(void);

    /** ���s���̏��� */
    SINT state_driving(void);

    /**
     * �C�x���g�t���O���Z�b�g����
     * @param ev �Z�b�g��Ԃɂ���C�x���g
     */
    void SetEventFlag(Robot::RobotEvent);
    bool IsSetEventFlag(Robot::RobotEvent);
    void ClearEventFlag(void);

    /** �X�e�[�^�X */
    enum DriverState {
        STATE_BT_CONNECT = 0,
        STATE_CALIBRATION,
        STATE_WAIT_START,
        STATE_DRIVING,
        STATE_COMPLETE,
        STATE_ERROR
    };

    DriverState status_;

    // �~�b�V�����֌W
    List<Mission*> mission_list_; //!< ���s����e�~�b�V����
    List<Mission*>::Iterator cur_mission_iter_; //!< mission_list_�̃C�e���[�^ @todo �Q�Ɣ͈͂�����̊֐��Ȃ̂ŃX�R�[�v����������

    Robot& robot_; //!< Driver �����삷�� Robot
    Navigator& navigator_;
    bool is_bt_connect_; //!< Bluetooth���ڑ���ԂȂ��true�A���ڑ��Ȃ�fals
    bool is_opened_; //!< ���s���Ȃ�true�B @todo �Q�Ɣ͈͂�����̊֐��Ȃ̂ŃX�R�[�v����������

    EventFlag event_flag_; //!< ���{�b�g�C�x���g

    static Driver* theInstance_; //!< Singleton �̂��߁A���g��ێ�����B
       
    Driver(void);
    ~Driver();

};

#endif
