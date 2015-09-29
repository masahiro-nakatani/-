#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"

// ���� ����
//�@��Y
// 80�`79�@�F�@���x�@100�i��������X�^�[�g�j
// 89�`88�@�F�@���x�@120�i�J�[�u180���i�s����{�j

//�@��Y
// 74�`75�@�F�@���x�@120�@OK������������
// 72�`73�@�F�@���x�@120�@OK������������

// �X�^�[�g���͍����C���g���[�X
// 90�x�^�[����͉E���C���g���[�X


#define R2D(X) ((X) * M_PI/180.0) // ���W�A������x
#define D2R(X) ((X) * 180.0/M_PI) // �x���烉�W�A��

//�W���C���I�t�Z�b�g�ύX�p�}�N����`
#define SPEEDDOWN   -10 // ���z��X�������O�X
#define SPEED_UP_   5   // ���z�O�X��������X

//�V�[�\���K�i�ύX���̊p�x�␳�}�N����`
#define STAIRS_ST_LEN  1000.0 // �p�x�␳���̒�������[mm]
#define STAIRS_DEF360  360    // �V�[�\�I����̕␳�l[��]
#define STAIRS_POINT   240    // �K�i��菈���J�n�p�x[��]

//�K�i��菈���J��Ԃ���
#define STAIRS_CNT 2

// �K�i���x
#define STAIRS_SPEED_ 110

//�K�i�^�C�}�l
#define STAIRS_TIM1_1 1000 //�Փˌ��m�J�n�^�C�}(1���)
#define STAIRS_TIM1_2 400  //�Փˌ��m�J�n�^�C�}(2��ڈȍ~)�F��i�ڂ̂͂�
#define STAIRS_TIM3   800 //�K�i���㏈������


class Mission_Stairs : public Mission {
private:
    enum LocalState{
        STAIRS_IN = 0,     // 0_�K�i�ՓˑO�i�p�x�␳�`�J�n�p�x�܂Łj
    	STAIRS_HIT,        // 1_�K�i���[�h�P�i�K�i�Փ˂܂�)  �b
    	STAIRS_UP,         // 2_�K�i���[�h�Q�i�K�i��蒆�j   �b�����͌J��Ԃ��i�R�j
    	STAIRS_UP2,        // 3_�K�i���[�h�R�i�K�i����j   �b
    	STAIRS_FAUL1,      // 4_�K�i���[�h�S�i�K�i��肫��������j
    	STAIRS_FAUL2,      // 5_�K�i���[�h�T�i�K�i��肫���Ă���^�[���܂Łj
		STAIRS_TURN,       // 6_�K�i���[�h�U�i90�x�^�[���j
    	STAIRS_LOW,       // 7_�K�i���[�h�V�i������܂Œ��i�j
		STAIRS_RECOVER,		// 8_�K�i���[�h�W�i���C���ɕ��A�j
		STAIRS_LOW2,       // 9_�K�i���[�h�X�i���C���g���[�X�j
    	STAIRS_END,        // 10_�K�i�����I��
    };
public:

    Mission_Stairs(void) : Mission() {
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        speed_ = STAIRS_SPEED_;
        local_status_ = STAIRS_IN;
    	//local_status_ = STAIRS_FAUL2;
    	//local_status_ = STAIRS_HIT;
    	tec_pid_.Init();
		is_state_move_ = true;
    	flag = 0;
    }

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;
    	switch(local_status_){
    	case STAIRS_IN :
			if (is_state_move_) {
				is_state_move_ = false;
				//tec_s_.Init(ri);
				tec_pid_.Init();
				//tec_s_.Init(ri);
				// �V�[�\�[�C�x���g�I���ʒu���X�^�[�g�ʒu����360���Ɛݒ肷��B
				// ��500mm�ŕ␳����B
		 		//Navigator::Instance().StartDirCorrection(STAIRS_ST_LEN,STAIRS_DEF360);
				timer_start(100);
			}
			cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);

    		//if( (ni.pos.Ang *180.0)/M_PI < STAIRS_POINT){
    		if(is_timeup()){
				local_status_ = STAIRS_HIT; //����STAIRS_HIT
				is_state_move_ = true;
				Robot::Instance().Beep(Robot::B);
    		}
    		break;
    	case STAIRS_HIT :
			// �K�i���[�h�J�n��A��P�b��PID�g���[�X���s���B
			// 1�b�o�ߌ�A�W���C���i�V�O�j�̗h��Ŏ��̃X�e�[�^�X��
			// �X�e�[�^�X3����̑J�ڂ̏ꍇ�A�^�C�}�l��ύX���ČJ��Ԃ��B
			if (is_state_move_) {
				is_state_move_ = false;
				if(flag==0){// 1���
					flag = 1;
					timer_start(STAIRS_TIM1_1);
				}else{      // 1��ڈȍ~
					if(flag < STAIRS_CNT){
						flag++;
						timer_start(STAIRS_TIM1_2);
					}else{
						is_state_move_ = true;
						local_status_ = STAIRS_FAUL1; //����STAIRS_FAUL1
					}
				}
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		Robot::Instance().Beep();
			
    		//�����ۂ��グ��
    		cmd.param3 = 0;
    		
			if (is_timeup() && evf.is_SetEvent(Robot::EV_GYRO)) {
				is_state_move_ = true;
				local_status_ = STAIRS_UP; //����2
				Robot::Instance().Beep(Robot::F);
			}
			break;
		case STAIRS_UP:
    		// �W���C���I�t�Z�b�g���]10���ĉ��z�I�ɑO�X�Ƃ���B
    		// ���K�i����낤�Ƃ���Ƃ��ɑO�X�Ɣ��f���������悤�Ƃ���̂��y��
    		
			if (is_state_move_) {
				is_state_move_ = false;
	    		Robot::Instance().SetGyro(-10);
				timer_start (STAIRS_TIM3);
				//tec_s_.Init(ri);
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		//cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);

    		if (is_timeup()) {
				local_status_ = STAIRS_UP2; //����3
				Robot::Instance().Beep(Robot::D);
				is_state_move_ = true;
			}
    		break;
    	case STAIRS_UP2 :
    		// �����͑O�X�e�[�^�X����̕ύX�P�񂾂��ʂ�̂ŏ����X�e�[�^�X�ύX�t���O����͕s�v
    		// ��莞�ԂőO�X�e�[�^�X�ŕύX�����I�t�Z�b�g������߂��B
    		// ���I�t�Z�b�g��ύX����ƁA��莞�Ԃŉ������͏I������B
    		//   �������A�I�t�Z�b�g�͕ύX�����܂܂Ȃ̂œ|���U�q���s����ȏ�ԂɂȂ邽�߁B
    		//   �������߂��Ȃ��Ƃ܂����������������邽�ߒ���
    		Robot::Instance().SetGyro(5);
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, STAIRS_SPEED_, Technic::LEFT);
    		//cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
			
    		local_status_ = STAIRS_HIT;
			break;
    	case STAIRS_FAUL1 :
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(100);
				Robot::Instance().ResetBalanceAPI();
				Navigator::Instance().ResetEncoder();
				tec_pid_.Init();
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, 80, Technic::LEFT);
    		//�O�̂��߁A�����ۂ��グ��
    		cmd.param3 = 0;
    		
    		if (is_timeup()) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_FAUL2;
    		}    		
    		break;
    	case STAIRS_FAUL2 :		//��肫����
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(2000);
			}
	    	cmd = tec_pid_.Calclate(ri.light_sensor_val, 80, Technic::LEFT);
    		cmd.param2 /= 5;
    		Robot::Instance().Beep(Robot::F);
    		if (is_timeup() && evf.is_SetEvent(Robot::EV_LIGHT_BLACK)) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_TURN;
    		}    		
    		break;
    		
    	case STAIRS_TURN :		// 90�x��]
			if (is_state_move_) {
				is_state_move_ = false;
				posBase1 = ni.pos;
				tec_r_.Init();
			}
	    	speed_ = 0;
			cmd = tec_r_.Calclate(speed_, 40, Technic::LEFT);
			if( ( ni.pos.Ang - posBase1.Ang ) > 0.49 * M_PI ){
				is_state_move_ = true;
				local_status_ = STAIRS_LOW;
			}
    		break;
    	case STAIRS_LOW:		// ������܂Œ��i
			if (is_state_move_) {
				is_state_move_ = false;
				timer_start(500);
				speed_ = 100;
				tec_s_.Init(ri);
				//tec_pid_.Init();
			}
			cmd = tec_s_.Calclate(speed_, ri.rot_encoderL_val, ri.rot_encoderR_val);
    			//cmd = tec_pid_.Calclate(ri.light_sensor_val, speed_, Technic::RIGHT);
    		if (is_timeup() && evf.is_SetEvent(Robot::EV_GYRO)) {
				is_state_move_ = true;
				local_status_ = STAIRS_RECOVER;
			}
    		break;
    	case STAIRS_RECOVER:		// ���A�����������Ɠ��ꂽ��
    		if (is_state_move_) {
				is_state_move_ = false;
				speed_ = 10;
    			timer_start(1000);
				tec_r_.Init();
			}
	    	cmd = tec_r_.Calclate(speed_, -5, Technic::RIGHT);
    			Robot::Instance().Beep(Robot::D);
			if ( is_timeup() && evf.is_SetEvent(Robot::EV_LIGHT_BLACK) ) {
				is_state_move_ = true;
				local_status_ = STAIRS_LOW2;
			}
    		break;
    	case STAIRS_LOW2 :
			if (is_state_move_) {
				is_state_move_ = false;
				tec_pid_.Init();
				timer_start(3000);
			}
    		
    		cmd = tec_pid_.Calclate(ri.light_sensor_val, 10, Technic::RIGHT);
    		Robot::Instance().Beep(Robot::D);
    		if (is_timeup()) {
    			is_state_move_ = true;
	    		local_status_ = STAIRS_END;
    		}    		
    		break;
		case STAIRS_END:
    	default :
	    	ret = false;
    		break;
    	}
    	cmd.param3 = 0;
        return ret;
    }

    	virtual char* Name(void){
        return "Mission_Stairs";
    }

private:

    S16 speed_;
	S16 rotation_;
	F32 angSum;
	S32 angNum;
	F32 angBase;
	bool is_state_move_;
    TecStraight tec_s_;
	TecPIDTrace tec_pid_;
	TecRotate tec_r_;
    S8 local_status_;
    int            flag;
    Position posBase1;
};
