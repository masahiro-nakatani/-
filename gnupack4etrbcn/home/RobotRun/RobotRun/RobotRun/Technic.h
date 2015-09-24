#ifndef _INCLUDE_TECHNIC_H_
#define _INCLUDE_TECHNIC_H_

#include "CourseInfo.h"
#include "PIDController.h"
#include "jiro_type.h"
#include "Robot.h"
#include "note.h"       /* 音階データ */
#include "music_data.h" /* 楽譜データ */

extern "C"
{
	#include "balancer.h"
};


//*****************************************************************
// PIDトレース
//*****************************************************************

class Technic {
public:
    enum DIRECTION{
        RIGHT = 0,
        LEFT
    };
};



class TecPIDTrace {
public:
	TecPIDTrace();
    void Init(void);
    RobotCmd  Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge);

private:
    F32 Rotation_;

    F32 E1_;
    F32 E2_;
    PIDParam PIDParam_;
};

//*****************************************************************
// ON/OFF制御
//*****************************************************************

class TecOnOffTrace {
public:
    void Init(void);
    RobotCmd  Calclate(S16 lightValue, S16 speed, S16 turn, Technic::DIRECTION edge);
};



//*****************************************************************
// 直進
//*****************************************************************

class TecStraight {
public:
    void Init(RobotInfo& ri);
    RobotCmd Calclate(S16 speed, S16 el, S16 er);

private:
    F32 Rotation_;

    S32 prev_EncR_;
    S32 prev_EncL_;

};

//*****************************************************************
// 直進2
//*****************************************************************

#if 0
class TecStraightWithAng {
public:
    void Init(NavInfo& ni);
    RobotCmd Calclate(S16 speed, NavInfo& ni);

private:
    F32 target_ang_;

};
#endif


//*****************************************************************
// 回転
//*****************************************************************



class TecRotate {
public:
    void Init(void);

    RobotCmd  Calclate(S16 speed, S16 turn_speed, Technic::DIRECTION edge);

private:
    F32 Rotation_;

};


//*****************************************************************
// パラメトリックカーブトレース
//*****************************************************************
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

//*****************************************************************
// 音楽演奏
//*****************************************************************
class TecMusic {
public:
    static TecMusic& Instance(void);
    void StartMusic(U8* score);
    void StopMusic(void);
    void Player(void);

private:
    enum MUSIC{
        STOP = 0,
        PLAY
    };

    TecMusic();
	int note2freq(unsigned char code);

    static TecMusic* theInstance_;
    U8 *musical_score;
	U8 music_flag;
};


#endif
