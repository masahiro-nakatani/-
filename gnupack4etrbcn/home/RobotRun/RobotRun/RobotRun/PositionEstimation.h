#ifndef __POSITION_ESTIMATION_H__
#define __POSITION_ESTIMATION_H__

#include "ecrobot_types.h"

#include <math.h>	//sin cos 計算で必要

typedef struct _Position{
	F64 X;		//位置X[mm]
	F64 Y;		//位置Y[mm]
	F64 Ang;	//角度[rad]
} Position;

void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now);




#endif /* __POSITION_ESTIMATION_H__*/

