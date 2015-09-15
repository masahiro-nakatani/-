#include "Technic.h"

/**
 * @file  Technic.cpp
 * @brief ロボットの動作パターンの実装
 */

TecMusic* TecMusic::theInstance_ = 0 ;


// PID


/** 
 * PIDControllerで定義された所定のpidパラメータで初期化する。速度角度はすべて0
 */
void TecPIDTrace::Init(void){
        Rotation_ = 0.0; 
        E1_ = 0.0;
        E2_ = 0.0;
//        PIDParam pid_param = { 0.38, 0.0, 0.027 };
//        PIDParam pid_param     = { 1.0, 0.0, 40.0 };
//        PIDParam_ = pid_param;

        PIDParam_ = pid_light_param;
}

/** 
 * 輝度値と走行速度から次の走行速度と回転角度を返す
 * 
 * @note 計算後の走行速度は計算前と同じ値を返している
 * @param lightValue 輝度値
 * @param speed 走行速度
 * @param edge 線のどちら側を走行するか(エッジ)
 * @param lightOffset ちょっと白よりにするパラメータ。
 * @return PIDで計算した走行速度と回転角度を RobotCmd として返す
 */
RobotCmd  TecPIDTrace::Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge, F32 lightOffset){

    F32 normalizedLightValue = (1.0 - (-1.0)) /
            (CourseInfo::colorBlack - CourseInfo::colorWhite) *
            (lightValue - CourseInfo::colorWhite) + (-1.0);

    F32 e0 = normalizedLightValue - lightOffset;     // ちょっと白より（灰色対策)

    F32 deltaRotation;

    pid_calc_rotation (&PIDParam_, e0, E1_, E2_, &deltaRotation);

    if ( edge == Technic::RIGHT ){
        Rotation_ += 0.3 * deltaRotation;
    } else {
        Rotation_ -= 0.3 * deltaRotation;
    }

    S16 accel = speed;


    RobotCmd ret((S16)accel, (S16)Rotation_);

    E2_ = E1_;
    E1_ = e0;

    return ret;

}

RobotCmd  TecPIDTrace::Calclate2(S16 lightValue, S16 speed, Technic::DIRECTION edge, F32 lightOffset){

    F32 normalizedLightValue = (1.0 - (-1.0)) /
            (CourseInfo::colorBlack - CourseInfo::colorGray) *
            (lightValue - CourseInfo::colorGray) + (-1.0);

    F32 e0 = normalizedLightValue - lightOffset;     // ちょっと白より（灰色対策)

    F32 deltaRotation;

    pid_calc_rotation (&PIDParam_, e0, E1_, E2_, &deltaRotation);

    if ( edge == Technic::RIGHT ){
        Rotation_ += 0.3 * deltaRotation;
    } else {
        Rotation_ -= 0.3 * deltaRotation;
    }

    S16 accel = speed;


    RobotCmd ret((S16)accel, (S16)Rotation_);

    E2_ = E1_;
    E1_ = e0;

    return ret;

}

//*****************************************************************
// ON/OFF制御 (未使用)
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

/**
 * 渡されたエンコーダ値を前回値として初期化
 */
void TecStraight::Init(RobotInfo& ri){
    Rotation_ = 0.0;
    prev_EncL_ = ri.rot_encoderL_val;
    prev_EncR_ = ri.rot_encoderR_val;
}

/**
 * 現在と前回のモータエンコーダ値から、直進するよう RobotCmd を返す。
 * @todo 直進の計算が未実装
 * @param speed 現在の速度
 * @param el    現在の左モータのエンコーダ値
 * @param er    現在の右モータのエンコーダ値
 * @return 計算した走行速度と回転角度を RobotCmd として返す
 */
RobotCmd TecStraight::Calclate(S16 speed, S16 el, S16 er){

    S32 tl = el - prev_EncL_;
    S32 tr = er - prev_EncR_;

    // 回転角度計算する。だれか実装して・・・
    //Rotation_ = tr - tl;
	Rotation_ = 0;

    if ( Rotation_ > 10 ){
        Rotation_ = 10;
    }

    RobotCmd ret((S16)speed, (S16)Rotation_);
    return ret;
}

//*****************************************************************
// 直進2 (未使用)
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

/** 
 * 初期化処理。何もしない。
 */
void TecRotate::Init(void){
}

/**
 * ロボットが回転するよう、PID制御なしで計算した RobotCmd を返す。
 * @note 計算後の走行速度は計算前と同じ値を返している
 * @note エッジの向きに応じて、turn_speed を足し引きしているだけ。
 * @param speed
 * @param turn_speed 
 * @param edge 線のどちら側を走行するか(エッジ)
 * @return 計算した走行速度と回転角度を RobotCmd として返す。
 */
RobotCmd  TecRotate::Calclate(S16 speed, S16 turn_speed, Technic::DIRECTION edge){

    VectorT<S8> cmd ; /** @todo cmdは未使用。 */

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

/**
 * インスタンスを返す。未生成なら新たに生成する。
 */
TecMusic& TecMusic::Instance(void){
	if( !theInstance_ ){
		theInstance_ = new TecMusic();
	}
	return *(theInstance_);
}

/** 
 * 演奏停止状態で初期化
 */
TecMusic::TecMusic(){
	music_flag = STOP;
}

/**
 * 演奏を開始する。
 * @param score 演奏する曲データ。
 *              音の高さと長さを交互に並べた配列。
 *              ただし、0番目の要素はテンポを表す。1番目の要素は無視される。
 */
void TecMusic::StartMusic(U8* score){
	music_flag = PLAY;
	musical_score = score;
}

/**
 * 演奏を一時停止する。
 */
void TecMusic::StopMusic(void){
	music_flag = STOP;
}


/**
 * 音楽を演奏する。
 * @note 1msごとに呼び出す必要がある
 */
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

/**
 * 音から周波数へ変更する
 * @code 1音分の音データ
 * @return 周波数
 */
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




