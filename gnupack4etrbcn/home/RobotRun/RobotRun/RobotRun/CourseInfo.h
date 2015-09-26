#ifndef _INCLUDE_COURCE_INFO_H_
#define _INCLUDE_COURCE_INFO_H_


#include "ecrobot_interface.h"

class Area{
public:
	F32 endX1;	//区間終了エリアx1
	F32 endX2;	//区間終了エリアx2
	F32 endY1;	//区間終了エリアy1
	F32 endY2;	//区間終了エリアy2
};

class CourseInfo {
public:
    static S16 colorBlack;
    static S16 colorWhite;
    static S16 colorGray;

	//スタート位置
	static F32 startPosXIn;
	static F32 startPosYIn;
	static F32 startPosAngIn;
	static F32 startPosXOut;
	static F32 startPosYOut;
	static F32 startPosAngOut;

	enum CourseType {
		InCourse,
		OutCourse,
	};

	static CourseType type;

	static F32 ratio;

	////アウトコース区間
	//enum{
	//	AREA_O_1ST_CORNER,
	//	AREA_O_SEESAW,
	//	AREA_O_1ST_STRAIGHT,
	//	AREA_O_STAIRS,
	//	AREA_O_LAST_CORNER,
	//	AREA_O_SLOPE,
	//	AREA_O_END,
	//	AREA_O_SIZE,
	//};

	////インコース区間
	//enum{
	//	//AREA_I_AQUERIUS,
	//	AREA_I_SHICANE,
	//	AREA_I_ENIGMA,
	//	AREA_I_LAST_CURVE,
	//	AREA_I_SLOPE,
	//	AREA_I_END,
	//	AREA_I_SIZE,
	//};


    //static bool Aquarius1;
    //static bool Aquarius2;
    //static bool Aquarius3;
	static bool PetBottle1;
	static bool PetBottle2;

    //static Area area[];
	//static int getAreaSize();
	static void getStartPos(F64& x, F64& y, F64& ang);
};


#endif
