#ifndef _INCLUDE_TECHNIC_H_
#define _INCLUDE_TECHNIC_H_

/**
 * @file  Technic.h
 * @brief ���{�b�g�̓���p�^�[���錾
 */

#include "CourseInfo.h"
#include "PIDController.h"
#include "InfoType.h"
#include "Robot.h"
#include "Note.h"      /* ���K�f�[�^ */
#include "MusicData.h" /* �y���f�[�^ */

extern "C"
{
	#include "balancer.h"
};


/**
 * ���C���g���[�X�̃G�b�W�̌������`
 * @todo �G�b�W�̌��������g���Ă��Ȃ����߃N���X������K�v���Ȃ��Bnamespace�ւ̒u����������������B
 */
class Technic {
public:
    /**
     * �G�b�W�̌������`
     */
    enum DIRECTION{
        RIGHT = 0,
        LEFT
    };
};

/**
 * PID�g���[�X
 */
class TecPIDTrace {
public:
    
    /** 
     * PIDController�Œ�`���ꂽ�����pid�p�����[�^�ŏ���������B���x�p�x�͂��ׂ�0
     */
    void Init(void);

    /** 
     * �P�x�l�Ƒ��s���x���玟�̑��s���x�Ɖ�]�p�x��Ԃ�
     */
    RobotCmd  Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge, F32 lightOffset = -0.3);

private:
    volatile F32 Rotation_; //!< ��]�p�x
    volatile F32 E1_; //!< 1�O�̋P�x�l�BCalclate���Ă΂��ƍX�V�����B
    volatile F32 E2_; //!< 2�O�̋P�x�l�BCalclate���Ă΂��ƍX�V�����B
    PIDParam PIDParam_; //!< PID�p�����[�^
};

/** 
 * ON/OFF����(?)
 * @todo ���g�p
 */
class TecOnOffTrace {
public:
    void Init(void);
    RobotCmd  Calclate(S16 lightValue, S16 speed, S16 turn, Technic::DIRECTION edge);
};


/** 
 * PID����Ȃ��Œ��i����B
 * ���[�^�̃��[�^���[�G���R�[�_�l���g�p����B
 */
class TecStraight {
public:
    /**
     * �n���ꂽ�G���R�[�_�l��O��l�Ƃ��ď�����
     */
    void Init(RobotInfo& ri);
    
    /**
     * ���݂ƑO��̃��[�^�G���R�[�_�l����A���i����悤 RobotCmd ��Ԃ��B
     */
    RobotCmd Calclate(S16 speed, S16 el, S16 er);
private:
    volatile F32 Rotation_; /**  @todo calclate ���ł����g�p����Ȃ��B�X�R�[�v���i��ׂ�  */
    volatile S32 prev_EncR_; //!< 1�O�̍����[�^�̃G���R�[�_�l�BCalclate���Ă΂��ƍX�V�����B
    volatile S32 prev_EncL_; //!< 1�O�̉E���[�^�̃G���R�[�_�l�BCalclate���Ă΂��ƍX�V�����B
};

#if 0
/** 
 * PID����Ȃ��Œ��i����B
 */
class TecStraightWithAng {
public:
    void Init(NavInfo& ni);
    RobotCmd Calclate(S16 speed, NavInfo& ni);

private:
    F32 target_ang_;

};
#endif

/**
 * ��]
 */
class TecRotate {
public:
    /** �����������B�������Ȃ��B */
    void Init(void);

    /**
     * ���{�b�g����]����悤�APID����Ȃ��Ōv�Z���� RobotCmd ��Ԃ��B
     */
    RobotCmd  Calclate(S16 speed, S16 turn_speed, Technic::DIRECTION edge);

private:
    volatile F32 Rotation_; /** @todo ���g�p */
};

//---------------------------------------------------------------------------
/*!
 * @brief   ���W������W�ւ̈ړ�
 * 
 *   �ڕW�x�N�g���ɑ΂���PID������s��
 */
//---------------------------------------------------------------------------
class TecPointToPoint
{
public:
	void Init(void){ }

	RobotCmd  Calclate(S16 speed, const Vec& to, Technic::DIRECTION edge);

};


/**
 * �p�����g���b�N�J�[�u�g���[�X
 * @todo �ڂ����l�R�����g���̂�
 */
class TecBezierTrace {
  public:
	void Init (NavInfo ni, const Vec* ctl, const S32* time, const S32 num, bool isStopMode = false);
	RobotCmd Calclate (Position& pos);
	bool isLastCtlPoints ();
	bool isFinished ();

  private:
	F32 CalcParamT (S32& nowTime);
	bool AlterCtlPoint (S32& nowTime);
	Vec CalcPoint (F32& t);
	Vec CalcDirection (F32& t);

	Position org_pos_;
	S32 org_time_;
	const Vec* ctl_point_;
	const S32* ctl_time_;
	S16 ctl_point_num_;
	bool isFinished_;
	bool isStopMode_;
};

/** 
 * ���y���t
 */
class TecMusic {
public:
    /**
     * �C���X�^���X��Ԃ��B�������Ȃ�V���ɐ�������B
     */
    static TecMusic& Instance(void);

    /**
     * ���t���J�n����B
     * @param score ���t����ȃf�[�^�B
     *              ���̍����ƒ��������݂ɕ��ׂ��z��B
     *              �������A0�Ԗڂ̗v�f�̓e���|��\���B1�Ԗڂ̗v�f�͖��������B
     */
    void StartMusic(U8* score);

    /**
     * ���t���ꎞ��~����B
     */
    void StopMusic(void);

    /**
     * ���y�����t����B
     * @note 1ms���ƂɌĂяo���K�v������
     */
    void Player(void);

private:
    enum MUSIC{ //!< �Đ����
        STOP = 0, //!< �Đ���
        PLAY      //!< ��~��
    };

    TecMusic();

    /**
     * ��������g���֕ύX����
     * @code 1�����̉��f�[�^
     * @return ���g��
     */
	int note2freq(unsigned char code);

    static TecMusic* theInstance_; //!< �V���O���g���p�̎��g�̃C���X�^���X
    U8 *musical_score; //!< ���t���̋ȃf�[�^
	U8 music_flag; //!< MUSIC::STOP �܂��� MUSCI::PLAY ��ێ�����
};

#endif
