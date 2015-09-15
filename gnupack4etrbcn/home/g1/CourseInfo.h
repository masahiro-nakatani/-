#ifndef _INCLUDE_COURCE_INFO_H_
#define _INCLUDE_COURCE_INFO_H_

/**
 * @file CourseInfo.h
 * @brief コース情報の宣言
 */

#include "ecrobot_interface.h"

/**
 * 区間情報
 */
class Area {
public:
	F32 endX1;	//!< 区間終了エリアx1
	F32 endX2;	//!< 区間終了エリアx2
	F32 endY1;	//!< 区間終了エリアy1
	F32 endY2;	//!< 区間終了エリアy2
};

/**
 * コース情報
 */
class CourseInfo {
public:
    static S16 colorBlack; //!< コースの黒値。キャリブレーションレーションで設定される。
    static S16 colorWhite; //!< コースの白値。キャリブレーションレーションで設定される。
    static S16 colorGray;  //!< @todo 未使用

	//スタート位置
	static F32 startPosXIn;    //!< インコース   スタート位置のX座標
	static F32 startPosYIn;    //!< インコース   スタート位置のY座標
	static F32 startPosAngIn;  //!< インコース   スタート位置における走行体の角度
	static F32 startPosXOut;   //!< アウトコース スタート位置のX座標				
	static F32 startPosYOut;   //!< アウトコース スタート位置のY座標				
	static F32 startPosAngOut; //!< アウトコース スタート位置における走行体の角度 
    
	enum CourseType { //!< インコース or アウトコース
		InCourse,
		OutCourse,
	};

	static CourseType type; 

	static F32 ratio; //!< 実寸座標/Inkscape座標.

    /**
     * ロボットの初期位置を返す
     */
	static void getStartPos(F64& x, F64& y, F64& ang);
};

#endif
