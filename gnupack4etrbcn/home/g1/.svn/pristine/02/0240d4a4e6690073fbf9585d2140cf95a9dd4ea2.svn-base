#ifndef _INCLUDE_TECHNIC_H_
#define _INCLUDE_TECHNIC_H_

/**
 * @file  Technic.h
 * @brief ロボットの動作パターン宣言
 */

#include "CourseInfo.h"
#include "PIDController.h"
#include "InfoType.h"
#include "Robot.h"
#include "Note.h"      /* 音階データ */
#include "MusicData.h" /* 楽譜データ */

extern "C"
{
	#include "balancer.h"
};


/**
 * ライントレースのエッジの向きを定義
 * @todo エッジの向きしか使っていないためクラス化する必要がない。namespaceへの置き換えを検討する。
 */
class Technic {
public:
    /**
     * エッジの向きを定義
     */
    enum DIRECTION{
        RIGHT = 0,
        LEFT
    };
};

/**
 * PIDトレース
 */
class TecPIDTrace {
public:
    
    /** 
     * PIDControllerで定義された所定のpidパラメータで初期化する。速度角度はすべて0
     */
    void Init(void);

    /** 
     * 輝度値と走行速度から次の走行速度と回転角度を返す
     */
    RobotCmd  Calclate(S16 lightValue, S16 speed, Technic::DIRECTION edge, F32 lightOffset = -0.3);

private:
    volatile F32 Rotation_; //!< 回転角度
    volatile F32 E1_; //!< 1つ前の輝度値。Calclateが呼ばれると更新される。
    volatile F32 E2_; //!< 2つ前の輝度値。Calclateが呼ばれると更新される。
    PIDParam PIDParam_; //!< PIDパラメータ
};

/** 
 * ON/OFF制御(?)
 * @todo 未使用
 */
class TecOnOffTrace {
public:
    void Init(void);
    RobotCmd  Calclate(S16 lightValue, S16 speed, S16 turn, Technic::DIRECTION edge);
};


/** 
 * PID制御なしで直進する。
 * モータのロータリーエンコーダ値を使用する。
 */
class TecStraight {
public:
    /**
     * 渡されたエンコーダ値を前回値として初期化
     */
    void Init(RobotInfo& ri);
    
    /**
     * 現在と前回のモータエンコーダ値から、直進するよう RobotCmd を返す。
     */
    RobotCmd Calclate(S16 speed, S16 el, S16 er);
private:
    volatile F32 Rotation_; /**  @todo calclate 内でしか使用されない。スコープを絞るべき  */
    volatile S32 prev_EncR_; //!< 1つ前の左モータのエンコーダ値。Calclateが呼ばれると更新される。
    volatile S32 prev_EncL_; //!< 1つ前の右モータのエンコーダ値。Calclateが呼ばれると更新される。
};

#if 0
/** 
 * PID制御なしで直進する。
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
 * 回転
 */
class TecRotate {
public:
    /** 初期化処理。何もしない。 */
    void Init(void);

    /**
     * ロボットが回転するよう、PID制御なしで計算した RobotCmd を返す。
     */
    RobotCmd  Calclate(S16 speed, S16 turn_speed, Technic::DIRECTION edge);

private:
    volatile F32 Rotation_; /** @todo 未使用 */
};

//---------------------------------------------------------------------------
/*!
 * @brief   座標から座標への移動
 * 
 *   目標ベクトルに対してPID制御を行う
 */
//---------------------------------------------------------------------------
class TecPointToPoint
{
public:
	void Init(void){ }

	RobotCmd  Calclate(S16 speed, const Vec& to, Technic::DIRECTION edge);

};


/**
 * パラメトリックカーブトレース
 * @todo 詳しい人コメントたのむ
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
 * 音楽演奏
 */
class TecMusic {
public:
    /**
     * インスタンスを返す。未生成なら新たに生成する。
     */
    static TecMusic& Instance(void);

    /**
     * 演奏を開始する。
     * @param score 演奏する曲データ。
     *              音の高さと長さを交互に並べた配列。
     *              ただし、0番目の要素はテンポを表す。1番目の要素は無視される。
     */
    void StartMusic(U8* score);

    /**
     * 演奏を一時停止する。
     */
    void StopMusic(void);

    /**
     * 音楽を演奏する。
     * @note 1msごとに呼び出す必要がある
     */
    void Player(void);

private:
    enum MUSIC{ //!< 再生状態
        STOP = 0, //!< 再生中
        PLAY      //!< 停止中
    };

    TecMusic();

    /**
     * 音から周波数へ変更する
     * @code 1音分の音データ
     * @return 周波数
     */
	int note2freq(unsigned char code);

    static TecMusic* theInstance_; //!< シングルトン用の自身のインスタンス
    U8 *musical_score; //!< 演奏中の曲データ
	U8 music_flag; //!< MUSIC::STOP または MUSCI::PLAY を保持する
};

#endif
