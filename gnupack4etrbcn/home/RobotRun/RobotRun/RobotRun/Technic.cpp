#include "Technic.h"

TecMusic* TecMusic::theInstance_ = 0 ;


//*****************************************************************
// PID
//*****************************************************************

TecPIDTrace::TecPIDTrace()
	: Rotation_(0.0)
	, E1_(0.0)
	, E2_(0.0)
	, PIDParam_(pid_light_param)
{
}

void TecPIDTrace::Init(void){
        Rotation_ = 0.0;
        E1_ = 0.0;
        E2_ = 0.0;
//        PIDParam pid_param = { 0.38, 0.0, 0.027 };
//        PIDParam pid_param     = { 1.0, 0.0, 40.0 };
//        PIDParam_ = pid_param;

        PIDParam_ = pid_light_param;
}

RobotCmd  TecPIDTrace::Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge){
    F32 normalizedLightValue = (1.0 - (-1.0)) /
            (CourseInfo::colorBlack - CourseInfo::colorWhite) *
            (lightValue - CourseInfo::colorWhite) + (-1.0);

    F32 e0 = normalizedLightValue - (-0.5);     // ちょっと白より（灰色対策)

    F32 deltaRotation;

    pid_calc_rotation (&PIDParam_, e0, E1_, E2_, &deltaRotation);

    if ( edge == Technic::RIGHT ){
        Rotation_ += 50.0 * deltaRotation;
    } else {
        Rotation_ -= 50.0 * deltaRotation;
    }

    S16 accel = speed;


    RobotCmd ret((S16)accel, (S16)Rotation_);

    E2_ = E1_;
    E1_ = e0;

    return ret;

}

//*****************************************************************
// ON/OFF制御
//*****************************************************************

void TecOnOffTrace::Init (void) {
	// None
}

RobotCmd TecOnOffTrace::Calclate (S16 light, S16 speed, S16 turn, Technic::DIRECTION edge) {
	if (light <= (CourseInfo::colorBlack + CourseInfo::colorWhite) / 2) {
		turn *= -1; // 白側なら左回り
	}
	if (edge == Technic::LEFT) {
		turn *= -1; // 左エッジなら反転
	}

	return RobotCmd(speed, turn);
}


//*****************************************************************
// 直進
//*****************************************************************

void TecStraight::Init(RobotInfo& ri){
    Rotation_ = 0.0;
    prev_EncL_ = ri.rot_encoderL_val;
    prev_EncR_ = ri.rot_encoderR_val;
}


RobotCmd TecStraight::Calclate(S16 speed, S16 el, S16 er){

    S32 tl = el - prev_EncL_;
    S32 tr = er - prev_EncR_;

    // 回転角度計算する。だれか実装して・・・
    Rotation_ = tr - tl;

    if ( Rotation_ > 10 ){
        Rotation_ = 10;
    }

    RobotCmd ret((S16)speed, (S16)Rotation_);
    return ret;
}

//*****************************************************************
// 直進2
//*****************************************************************

#if 0
void TecStraightWithAng::Init(NavInfo& ni){
    target_ang_ = ni.pos.Ang;
}


RobotCmd TecStraightWithAng::Calclate(S16 speed, NavInfo& ni){
	S16 turn = (S16)(50.0 * (ni.pos.Ang - target_ang_) / (45.0/180.0*M_PI));
    RobotCmd ret((S16)speed, (S16)turn);
    return ret;
}
#endif

//*****************************************************************
// 回転
//*****************************************************************

void TecRotate::Init(void){
}


RobotCmd  TecRotate::Calclate(S16 speed, S16 turn_speed, Technic::DIRECTION edge){

    VectorT<S8> cmd ;

    S16 rot;
    if ( edge == Technic::RIGHT ){
        rot = turn_speed;
    } else {
        rot = -turn_speed;
    }

    RobotCmd ret(speed,rot);

    return ret;

}

//*****************************************************************
// 音楽演奏
//*****************************************************************
TecMusic& TecMusic::Instance(void){
	if( !theInstance_ ){
		theInstance_ = new TecMusic();
	}
	return *(theInstance_);
}

TecMusic::TecMusic(){
	music_flag = STOP;
}

void TecMusic::StartMusic(U8* score){
	music_flag = PLAY;
	musical_score = score;
}

void TecMusic::StopMusic(void){
	music_flag = STOP;
}

void TecMusic::Player(void){
	static int index =   0;
	static int count =   0;
	int        freq  = 440;
	int        tempo = 120;
	if (music_flag == PLAY) {
		if (index == 0) {
			tempo = (int)musical_score[0];
			index = 2;
		}
		if (count == 0) {
			freq = note2freq(musical_score[index]);
			count = ((int)musical_score[index+1] * 1250 / tempo);
			if (REST != freq) {
				Robot::Instance().Beep(freq, count - 2, 100U);
			}
			index += 2;
		}
		count--;
		if (musical_score[index] == 0) {
			index = 0;
			count = 0;
			music_flag = STOP;
		}
	}
}

int TecMusic::note2freq(unsigned char code) {
    unsigned short freq = 0;
    switch (0xf0 & code) {
        case  0x10 : freq = notes[(0x0f & code)] >> 7; break;
        case  0x20 : freq = notes[(0x0f & code)] >> 6; break;
        case  0x30 : freq = notes[(0x0f & code)] >> 5; break;
        case  0x40 : freq = notes[(0x0f & code)] >> 4; break;
        case  0x50 : freq = notes[(0x0f & code)] >> 3; break;
        case  0x60 : freq = notes[(0x0f & code)] >> 2; break;
        case  0x70 : freq = notes[(0x0f & code)] >> 1; break;
        case  0x80 : freq = notes[(0x0f & code)];      break;
        default    : freq = 0;                         break;
    }
    return (int)freq;
}
