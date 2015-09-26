#include "CourseInfo.h"


S16 CourseInfo::colorBlack = 0;
S16 CourseInfo::colorWhite = 0;
S16 CourseInfo::colorGray = 0;

F32 CourseInfo::ratio = (4770.0/1707.672); // 実寸座標/Inkscape座標.

//////インコース設定
#if 0
F32 CourseInfo::startPosXIn = 1807.8125 * CourseInfo::ratio;
F32 CourseInfo::startPosYIn = 177.26379 * CourseInfo::ratio;
#endif
F32 CourseInfo::startPosXIn = 5049.7202;
F32 CourseInfo::startPosYIn = 484.2356;
F32 CourseInfo::startPosAngIn = 3.141592;
//
////init area info
//Area CourseInfo::area[AREA_I_SIZE] = {/*{1010,1310,2870,3270}
//									,*/{3318,3330,2880,3280}
//									,{3840,4140,1400,1800}
//									,{4180,4480,300,700}
//									,{2100,2400,300,700}};


//アウトコース設定
F32 CourseInfo::startPosXOut = 4261.9292;
F32 CourseInfo::startPosYOut = 255.0996;
F32 CourseInfo::startPosAngOut = 3.141592;
//init area info
#if 0
Area CourseInfo::area[AREA_O_SIZE] = {{1200,1280,3100,3500}
									,{2650,2950,3100,3500}
									,{5000,2500,5280,2850}
									,{4950,5350,1100,1400}
									,{4200,4500,50,450}
									,{1990,2290,50,450}};
#endif

//bool CourseInfo::Aquarius1 = false;
//bool CourseInfo::Aquarius2 = false;
//bool CourseInfo::Aquarius3 = false;
bool CourseInfo::PetBottle1 = false;
bool CourseInfo::PetBottle2 = false;

CourseInfo::CourseType CourseInfo::type = CourseInfo::InCourse;

//int CourseInfo::getAreaSize(){
//	return AREA_O_END;
//}
//
void CourseInfo::getStartPos(F64 &x, F64 &y, F64 &ang){
	if (CourseInfo::type == CourseInfo::InCourse) {
		x = 0;
		y = 0;
		ang = 0;
	} else {
		x = 0;
		y = 0;
		ang = 0;
	}
}
