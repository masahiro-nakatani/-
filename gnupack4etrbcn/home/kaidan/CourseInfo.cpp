#include "CourseInfo.h"

/**
 * @file CourseInfo.cpp
 * @brief コース情報の定義
 */

S16 CourseInfo::colorBlack = 0; //!< コースの黒値。キャリブレーションレーションで設定される。
S16 CourseInfo::colorWhite = 0; //!< コースの白値。キャリブレーションレーションで設定される。
S16 CourseInfo::colorGray  = 0; //!< @todo 未使用

// 全体コース図の緑エリア左上の座標をx=0,y=0とし、実寸法(単位:mm)とする
// ET2013インコースのパラメータ
F32 CourseInfo::startPosXIn = 5108;
F32 CourseInfo::startPosYIn = 506;
F32 CourseInfo::startPosAngIn = 3.14159;

// ET2013アウトコースのパラメータ
F32 CourseInfo::startPosXOut = 4314;
F32 CourseInfo::startPosYOut = 256;
F32 CourseInfo::startPosAngOut = 3.14159;

CourseInfo::CourseType CourseInfo::type = CourseInfo::InCourse;

/**
 * ロボットの初期位置を返す
 * @param[out] x
 * @param[out] y
 * @param[out] ang
 */
void CourseInfo::getStartPos(F64 &x, F64 &y, F64 &ang){
	if (CourseInfo::type == CourseInfo::InCourse) {
		x = startPosXIn;
		y = startPosYIn;
		ang = startPosAngIn;
	} else {
		x = startPosXOut;
		y = startPosYOut;
		ang = startPosAngOut;
	}
}


