
#include "utils.h"
#include "debug.h"
#include "Driver.h"
#include "Robot.h"
#include "Mission.h"
#include "CourseInfo.h"
#include "Navigator.h"
#include "Technic.h"

// BS��Y����p�~�b�V������`
#ifdef ICHIRO
// �C���R�[�X
# ifdef IN_COURSE
#  include "Mission_TailStraight.cpp"
# else
// �A�E�g�R�[�X
#  include "Mission_TailStraight.cpp"
# endif

#else

// BS��Y����p�~�b�V������`
// �C���R�[�X
# ifdef IN_COURSE
#  include "Mission_TailStraight.cpp"
# else
// �A�E�g�R�[�X
#  include "Mission_TailStraight.cpp"
# endif
#endif

// Driver�^�X�N�̋N������[msec]
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
// �֐���	: user_1ms_isr_type2
// ����	: �Ȃ�
// �߂�l	: �Ȃ�
// �T�v	: 1msec�������荞�݃t�b�N�֐�(OSEK ISR type2�J�e�S��)
//=============================================================================
void user_1ms_isr_type2(void)
{
	(void)SignalCounter(SysTimerCnt); // Alarm�n���h��
	TecMusic::Instance().Player();
	SleeperMonitor(); // Nxt�����I2C�f�o�C�X�̎g�p���ɕK�v
}

//*************************************************************************
// �^�X�N�G���g��
//*************************************************************************
TASK(TaskDrive){

	// �R�[�X���ݒ�
	
	Driver::MissionPtr ptr[] = {
		new Mission_TailStraight()
	};

	enum{ missoin_size = sizeof(ptr)/sizeof(ptr[0]) };

#ifdef IN_COURSE
	// �C���R�[�X
	CourseInfo::type = CourseInfo::InCourse;
	Driver::Instance().AddMission(ptr, missoin_size);
#else
	// �A�E�g�R�[�X
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
// �R���X�g���N�^
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
// �f�X�g���N�^
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
// �h���C�o�N���X���C���v���Z�X
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
// Mission�ǉ�
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
// Mission�N���A
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
// ���{�b�g����̃C�x���g�ʒm��M
//*************************************************************************

void Driver::Notify(Robot::RobotEvent ev){
    this->SetEventFlag(ev);
}

//*************************************************************************
// ���ȏЉ��
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

    // �^�b�`�Z���T�����҂�
    this->ClearEventFlag();
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
		// Do nothing.
        Sleep_msec(10);
	}
	
    robot_.Beep();

    return 0;
}

//*************************************************************************
// Bluetooth�R�l�N�V��������
//*************************************************************************
SINT Driver::state_bt_connect(void){
    SINT ret;

    ret = robot_.BTConnect();
    switch (ret){
    case 1: //�R�l�N�g����
        this->is_bt_connect_ = true;
        break;
    default:
        this->is_bt_connect_ = false;
    }

    this->status_ = ST_CALIBRATION;
    return 0;
}

//*************************************************************************
// �L�����u���[�V��������
//*************************************************************************
SINT Driver::state_calibration(void){

    //******************
    // ���F�l������
    robot_.GetLCD().clear();
    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "Input Black");
    robot_.GetLCD().disp();

    RobotInfo ri;
    // �^�b�`�Z���T�����҂�
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
    // ���F�l������
    robot_.GetLCD().clear();
    robot_.GetLCD().cursor(0,0);
    robot_.GetLCD().putf("s", "Input White");
    robot_.GetLCD().disp();

    // �^�b�`�Z���T�����҂�
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

#define TAIL_ANGLE_STAND_UP  106 /* ���S��~���̊p�x[�x] */

//*************************************************************************
// �X�^�[�g�҂�����
//*************************************************************************
SINT Driver::state_wait_start(void){

    RobotInfo ri;

    robot_.GetLCD().clear();
	robot_.GetLCD().putf("s", "TOUCH: Set Gyro Offset");
	robot_.GetLCD().disp();

    // �^�b�`�Z���T�����҂�
    this->ClearEventFlag();
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
        robot_.GetLCD().cursor(1, 2);
        ri = robot_.GetInfo();
        robot_.GetLCD().putf("sd", "Set: ", ri.gyro_sensor_val, 0);
        robot_.GetLCD().disp();
        Sleep_msec(100);
    }
    //�W���C���L�����u���[�V����
    robot_.Calibration();
    Sleep_msec(500);
    robot_.Beep();

	robot_.GetLCD().clear();
	robot_.GetLCD().putf("s", "TOUCH: Reset Tail");
	robot_.GetLCD().disp();

    // �^�b�`�Z���T�����҂�
    this->ClearEventFlag();
    while( !(is_SetEventFlag(Robot::EV_TOUCH_SENSOR_ON)) ){
        Sleep_msec(100);
    }
	// �����ۃN���A
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
// �^�]������
//*************************************************************************
SINT Driver::state_driving(void){

    RobotCmd cmd;

	navigator_.Init();
	RobotInfo r_info = robot_.GetInfo();
	NavInfo n_info = navigator_.GetInfo(r_info);
    // �ŏ��̃~�b�V������ݒ聕������
    if ( current_index_ != mission_size_ )
	{
        mission_list_[current_index_]->Init(r_info, n_info);
    } else 
	{
        this->status_ = ST_ERROR;
    }

    this->ClearEventFlag();

    SetRelAlarm(Alarm4msec, 1, DRIVE_PERIOD); // �h���C�u�pAlarm�^�C�}�[�̃Z�b�g

    // ���s�J�n
    // �X�e�[�^�X���^�]���Ȃ烋�[�v���J��Ԃ�
    while(this->status_ == ST_DRIVING)
	{
        

        ClearEvent(EventDrive); // �h���C�u�C�x���g�̃N���A
		WaitEvent(EventDrive);  // �h���C�u�C�x���g�҂�

        // �^�b�`�Z���TON,�������͓]�|����葖�s��~
        if ( event_flag_.is_SetEvent(Robot::EV_TOUCH_SENSOR_ON) || event_flag_.is_SetEvent(Robot::EV_FAIL) ){
            status_ = ST_WAIT_START;
            break;
        }

        r_info = robot_.GetInfo();
        n_info = navigator_.GetInfo(r_info);

#if 1
        // Bluetooth �f�[�^���M
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
                // ���̃~�b�V�����ɑJ��
                ++current_index_;
                if ( current_index_ == mission_size_ ){
                    // �~�b�V�������X�g���Ȃ��Ȃ����̂ŏI��
                    this->status_ = ST_COMPLETE;
                    break;
                } else {
                    mission_list_[current_index_]->Init(r_info, n_info);
                    continue;
                }
            } 

			// Bluetooth �f�[�^���M
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
    CancelAlarm(Alarm4msec);            // �h���C�u�pAlarm�^�C�}�[�̃L�����Z��
    ClearEvent(EventDrive); // �h���C�u�C�x���g�̃N���A
    return 0;
}


//*************************************************************************
// �C�x���g�Z�b�g
//*************************************************************************
void Driver::SetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    this->event_flag_.Set(ev);
}


//*************************************************************************
// �C�x���g�m�F
//*************************************************************************
bool Driver::is_SetEventFlag(Robot::RobotEvent ev){
//    Lock l(Resource_DEList);
    return this->event_flag_.is_SetEvent(ev);
}

//*************************************************************************
// �C�x���g�m�F
//*************************************************************************
void Driver::ClearEventFlag(void){
//    Lock l(Resource_DEList);
    this->event_flag_.Clear();
}
