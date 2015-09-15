#include "TouchSensor.h"
#include "SonarSensor.h"
#include "LightSensor.h"
#include "GyroSensor.h"
#include "Motor.h"
#include "Nxt.h"
#include "Lcd.h"
#include "Daq.h"
#include "Bluetooth.h"
#include "BTConnection.h"


#include "List.h"
#include "Utils.h"
#include "Debug.h"

#include "Robot.h"
#include "CourseInfo.h"

extern "C"
{
	#include "balancer.h"
};

using namespace ecrobot;
using namespace nro;

//***************************************
// �f�o�C�X�I�u�W�F�N�g��`
// �ȉ��̃I�u�W�F�N�g�̓O���[�o���C���X�^���X�Ƃ��Đ�������K�v������炵��
// (ECRobot C++ API �}�j���A�����)


TouchSensor touch(PORT_4);
SonarSensor sonar(PORT_2);
LightSensor light(PORT_3);
GyroSensor   gyro(PORT_1);
Motor      motorL(PORT_C);
Motor      motorR(PORT_B);
Motor	     tail(PORT_A);
Bluetooth bt;
Lcd lcd;
Nxt nxt;
Speaker speaker;

MarkerCheck marker_check;

Robot* Robot::theInstance_ = 0;

extern "C"{

//*************************************************************************
// �R���X�g���N�^
//*************************************************************************
Robot::Robot(void) :
        current_state(None),
        touch_sensor_(touch),
        sonar_sensor_(sonar),
        light_sensor_(light),
        gyro_sensor_(gyro),
        motorL_(motorL),
        motorR_(motorR),
		tail_(tail),
        bluetooth_(bt),
        lcd_(lcd),
        nxt_(nxt),
        speaker_(speaker),
        daq_(bluetooth_),
        gyro_offset_(0),
        tail_angle_(0),
        bt_connect_state_(0),
		marker_check_(marker_check)
{
    this->Stop();
}


//*************************************************************************
// �f�X�g���N�^
//*************************************************************************
Robot::~Robot(void){
}


//********************************
// �^�X�N�G���g��
//********************************
TASK(TaskRobot){
    Robot::Instance().Process();
    TerminateTask();
}


}


//*************************************************************************
// �C���X�^���X�擾
//*************************************************************************
Robot& Robot::Instance(void){
    if (!theInstance_){
        theInstance_ = new Robot();
    }
    return *(theInstance_);
}


//*************************************************************************
// ���C���v���Z�X
//*************************************************************************
void Robot::Process(void){
    while(1){
        this->CheckTouchSensor();
        this->CheckSonarSensor();
        this->CheckGyroSensor();
        this->CheckLightSensor();
        this->CheckRemoteStart();
        Sleep_msec(20);
    }
}



//*************************************************************************
// ���[�^�[�o�͌v�Z
//*************************************************************************

//#define TURN_OFFSET -3
#define TURN_OFFSET 0

static S8 logL = 0;
static S8 logR = 0;

VectorT<S8> Robot::calcPWM(VectorT<S16> cmd){
	// save internal data
    //S16 gyro_data;
    S16 batt_data;
    S32 countL;
    S32 countR;

    gyro_data = this->gyro_sensor_.get();
    batt_data = this->nxt_.getBattMv();
    countL = motorL_.getCount();
    countR = motorR_.getCount();

	VectorT<S8> pwm;
	balance_control(
		static_cast<F32>(cmd.mX),
		static_cast<F32>(cmd.mY+TURN_OFFSET),
		static_cast<F32>(gyro_data),
		static_cast<F32>(this->gyro_offset_),
		static_cast<F32>(countL),
		static_cast<F32>(countR),
		static_cast<F32>(batt_data),
		&pwm.mX,
		&pwm.mY);

    logL = pwm.mX;
    logR = pwm.mY;

    return pwm;

}



//*************************************************************************
// �^�b�`�Z���T�|�[�����O����
//*************************************************************************
void Robot::CheckTouchSensor(void){
    bool ts = this->touch_sensor_.isPressed();

    if (this->touch_sensor_state_ != ts){
        this->touch_sensor_state_ = ts;

        if ( this->touch_sensor_state_ ){
            CallNotify(EVENT_TOUCH_SENSOR_ON);
        }
    }

}

//*************************************************************************
// �\�i�[�Z���T�|�[�����O����
//*************************************************************************
#define RANGE 50
void Robot::CheckSonarSensor(void){
	sonar_distance = this->sonar_sensor_.getDistance();
    if (sonar_distance <= RANGE){
        CallNotify(EVENT_SONAR_SENSOR_ON);
    }


}

//*************************************************************************
// �W���C���Z���T�|�[�����O����
//*************************************************************************
void Robot::CheckGyroSensor(void){

    S16 val= this->gyro_sensor_.get() - this->gyro_offset_;

    // �]�|���m�@
    // �W���C���Z���T�l��300�ȏ�u������]�|�������Ƃɂ���B
    if ( (val < -300) || (300 < val) ){
        CallNotify(EVENT_FAIL);
    }
    // 50�ȏ�u������Ȃɂ���Q�������������Ƃɂ���B
	else if ( (val < -70) || (70 < val) ){
        CallNotify(EVENT_GYRO);
    }
    // 20�ȏ�u������Ȃɂ���Q�������������Ƃɂ���B
    else if ( (val < -60) || (60 < val) ){
        CallNotify(EVENT_GYRO2);
    }
    // 50�ȏ�u������Ȃɂ���Q�������������Ƃɂ���B
    else if ( (val < -50) || (50 < val) ){
        CallNotify(EVENT_GYRO3);
    }



}


//*************************************************************************
// ���Z���T�|�[�����O����
//*************************************************************************
void Robot::CheckLightSensor(void){

    val_light = this->light_sensor_.get();
    S16 point = (CourseInfo::colorBlack - CourseInfo::colorWhite);
    point /= 2;
    point /= 2;

    if ( ( CourseInfo::colorBlack - point) < val_light ){
        CallNotify(EVENT_LIGHT_BLACK);
        CallNotify(EVENT_LIGHT_HAZARD);
    } else if ( ( CourseInfo::colorWhite + point) > val_light ){
        CallNotify(EVENT_LIGHT_HAZARD);
    }

	if (marker_check.IsMarker(val_light) == 1) {
		CallNotify(EVENT_MARKER);
	}
}


//*************************************************************************
// Bluetooth�����[�g�X�^�[�g�|�[�����O����
//*************************************************************************
enum eState
{
	eState_waittingComandHead,		// �R�}���h�w�b�_�҂���
	eState_waittingComandType,		// �R�}���h��ʑ҂���
	eState_waittingLengthHigh,		// �t���f�[�^�T�C�Y(���)�҂���
	eState_waittingLengthLow,		// �t���f�[�^�T�C�Y(����)�҂���
};
void Robot::CheckRemoteStart(void){
	U32		rxLen;								// ��M�f�[�^��
	U8		rxBuf[BT_MAX_TX_BUF_SIZE];			// ��M�o�b�t�@
	U16		rxLoc = 0;							// ��M�f�[�^�ʒu
	U16		addDataLen = 0;						// �t���f�[�^��
	U8		state = eState_waittingComandHead;	// ���
	U8		recvData;
	bool	isComplete = false;

	if( bt_connect_state_ == 1 )
	{
		rxLen = bt.receive(rxBuf, 0, BT_MAX_TX_BUF_SIZE);
	}
	else
	{
		rxLen = 0;
	}

	while(rxLen > 0)
	{
		recvData = rxBuf[rxLoc];
		switch( state )
		{
			case eState_waittingComandHead:			// �R�}���h�w�b�_�҂�
				if(recvData == 'C')
				{
					state = eState_waittingComandType;
				}
				break;
			case eState_waittingComandType:			// �R�}���h��ʑ҂�
				if(recvData == '0')
				{
					state = eState_waittingLengthHigh;
				}
				else
				{
					state = eState_waittingComandHead;
				}
				break;
			case eState_waittingLengthHigh:			// �t���f�[�^�T�C�Y(���)�҂�
				addDataLen = recvData;
				state = eState_waittingLengthLow;
				break;
			case eState_waittingLengthLow:			// �t���f�[�^�T�C�Y(����)�҂�
				addDataLen += (recvData << 8);
				if(addDataLen == 0)
				{
					CallNotify(EVENT_REMOTE_START);
					// ��M����
					isComplete = true;
				}
				state = eState_waittingComandHead;
				break;
			default:
				break;
		}
		rxLoc++;
		rxLen--;
		if(isComplete)
		{
			break;
		}
	}
}


//*************************************************************************
// �C�x���g�ʒm
//*************************************************************************
void Robot::CallNotify(RobotEvent e){

    List<RobotObserver*>::Iterator iter;

    for (iter = this->observer_list_.Begin(); iter != this->observer_list_.End(); ++iter){
        (*iter)->Notify(e);
    }
}


//*************************************************************************
// Bluetooth�f�[�^���M
//*************************************************************************
void Robot::BTSend(RobotCmd cmd, RobotInfo ri, NavInfo ni){
//void Robot::BTSend(RobotCmd cmd, Position pos){
    S8 dataS08[2];
    U16 dataU16;
    S16 dataS16[4];
    S32 dataS32[4];

//    dataS08[0] = static_cast<S8>(cmd.param1);       // (Data1)
//    dataS08[1] = static_cast<S8>(cmd.param2);       // (Data2)
    dataS08[0] = static_cast<S8>(logR);       // (Data1)
    dataS08[1] = static_cast<S8>(logL);       // (Data2)

    dataU16 = ri.battery_val;              // (Battery)

    dataS32[0]  = ri.rot_encoderT_val;     // (Motor Rev A) �K��
    dataS32[1]  = ri.rot_encoderL_val;     // (Motor Rev B) �����[�^���G���R�[�_�l
    dataS32[2]  = ri.rot_encoderR_val;     // (Motor Rev C) �E���[�^���G���R�[�_�l


	dataS16[0] = static_cast<S16>(val_light);       // (ADC S1) ���Z���T
    dataS16[1] = static_cast<S16>(gyro_data);        // (ADC S2) �W���C���Z���T
    dataS16[2] = static_cast<S16>(ni.pos.X); // (ADC S3) x���W
	dataS16[3] = static_cast<S16>(ni.pos.Y); // (ADC S3) y���W

//    F32 tmp = ((ni.pos.Ang * 180.0) / 3.141592);
//	dataS32[3] = static_cast<S32>(tmp);
	//dataS32[3] = static_cast<S16>(sonar_distance);          // (I2C)�����g�Z���T

    dataS32[3] = current_state;
    this->daq_.send(dataS08, dataU16, dataS16, dataS32);
}


static const CHAR BT_PASS_KEY[] =  "1234";
//*************************************************************************
// Bluetooth�R�l�N�g
//*************************************************************************
SINT Robot::BTConnect(void){
	BTConnection btConnection(bluetooth_, lcd_, nxt_);

  // 2013ET���{�R�����Z�K��(Buletooth�ʐM)�ɏ]��ET+�`�[��ID�Ƃ���
	this->bt_connect_state_ = btConnection.connect(BT_PASS_KEY, "ET14");

	return this->bt_connect_state_;
}


//*************************************************************************
// ������(�W���C���I�t�Z�b�g�L�����u���[�V�����w��)
//*************************************************************************

void Robot::Calibration(void){

	U32 calGyroOffset = 0;
	SINT numOfAvg = 0;
	while (numOfAvg < 100) // it takes approximately 4*100 = 400msec
	{
		calGyroOffset += this->gyro_sensor_.get();
		numOfAvg++;
		U32 tic = systick_get_ms();
		while (systick_get_ms() < 4 + tic); // wait for 4msec
	}

	this->gyro_offset_ = static_cast<S16>(calGyroOffset/numOfAvg);

    this->Stop();
    DEBUG_PRINT1("s", "Stop() >");

    balance_init();
    DEBUG_PRINT1("s", "blance_init() >");

}


//*************************************************************************
// ���s�w��
//*************************************************************************
#define MOFFSET 1.11
#define MOFFSETR 1.0	

void Robot::Drive(RobotCmd cmd){

    VectorT<S8> ret;

    if ( cmd.Mode == RobotCmd::NORMAL_MODE ){
    	VectorT<S16> motorPwm;
		motorPwm.mX = cmd.param1;
		motorPwm.mY = cmd.param2;

        ret = this->calcPWM(motorPwm);
	} else {
        ret.mX = cmd.param1;
		ret.mY = cmd.param2;
	}

    F32 x = (F32)(ret.mX)*MOFFSET;
    F32 y = (F32)(ret.mY)*MOFFSETR;


    this->motorL_.setPWM((S8)x);
    this->motorR_.setPWM((S8)y);
	this->TailControl(cmd);
}

//*****************************************************************
// �����ې���
//*****************************************************************

#define P_GAIN             2.5F /* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX          60 /* ���S��~�p���[�^����PWM��΍ő�l */

void Robot::TailControl(RobotCmd& cmd) {
	// ��3�p�����[�^�Ɏw�肪����΁A�ڕW�l��ύX
	// �w�����Ȃ���΁A�O��ݒ�l��ڕW�l�Ƃ���
	if (cmd.param3 != RobotCmd::NO_TAIL_CNTL) {
		tail_angle_ = cmd.param3;
	}

	float pwm = (float)(this->tail_angle_ - this->tail_.getCount())*P_GAIN; /* ��ᐧ�� */

	/* PWM�o�͖O�a���� */
	if (pwm > PWM_ABS_MAX) {
		pwm = PWM_ABS_MAX;
	} else if (pwm < -PWM_ABS_MAX) {
		pwm = -PWM_ABS_MAX;
	}

	this->tail_.setPWM((S16)pwm);
}

//*************************************************************************
// ��~�w��
//*************************************************************************
void Robot::Stop(void){
    this->motorL_.reset();
    this->motorR_.reset();
	this->tail_.reset();
}

//*************************************************************************
//
//*************************************************************************
void Robot::ResetBalanceAPI(void){

	balance_init();
    this->motorL_.reset();
    this->motorR_.reset();
}

//*************************************************************************
// ���擾
//*************************************************************************
RobotInfo Robot::GetInfo(void){
    RobotInfo ri;

    ri.light_sensor_val = this->light_sensor_.get();
    ri.rot_encoderL_val = this->motorL_.getCount();
    ri.rot_encoderR_val = this->motorR_.getCount();
    ri.rot_encoderT_val = this->tail_.getCount();
    ri.gyro_sensor_val = this->gyro_sensor_.get();
	ri.battery_val = this->nxt_.getBattMv();
    return ri;
}

S16 Robot::GetGyroOffset()
{
	return gyro_offset_;
}

void Robot::SetGyroOffset(S16 gyro){
	gyro_offset_ = gyro;
}

//*************************************************************************
// �W���C���I�t�Z�b�g�擾
//*************************************************************************
S16 Robot::GetGyro(void){
	return(this->gyro_offset_);
}

//*************************************************************************
// �W���C���I�t�Z�b�g�ݒ�
//*************************************************************************
S16 Robot::SetGyro(S16 up){
	this->gyro_offset_ += up;
	return(this->gyro_offset_);
}

//*************************************************************************
// �I�u�U�[�o�ǉ�
//*************************************************************************
void Robot::AddObserver(RobotObserver* obs){
    this->observer_list_.PushBack(obs);
}

//*************************************************************************
// �I�u�U�[�o�폜
//*************************************************************************
void Robot::RemoveObserver(RobotObserver* obs){

    List<RobotObserver*>::Iterator iter;

    iter = this->observer_list_.Find(obs);

    if ( iter != this->observer_list_.End() ){
        this->observer_list_.Remove(iter);
    }

}

//*************************************************************************
// ���[�^�̃G���R�[�_�l�擾
//*************************************************************************
void Robot::GetMotorEncoder(S32 &lmotor,S32 &rmotor){
	lmotor = this->motorL_.getCount();
	rmotor = this->motorR_.getCount();
}


