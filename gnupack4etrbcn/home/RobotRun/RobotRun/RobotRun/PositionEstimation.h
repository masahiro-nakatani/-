#ifndef __POSITION_ESTIMATION_H__
#define __POSITION_ESTIMATION_H__

#include "ecrobot_types.h"

#include <math.h>	//sin cos �v�Z�ŕK�v

typedef struct _Position{
	F64 X;		//�ʒuX[mm]
	F64 Y;		//�ʒuY[mm]
	F64 Ang;	//�p�x[rad]
} Position;

void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now);




#endif /* __POSITION_ESTIMATION_H__*/

