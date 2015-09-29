#pragma once



typedef          char CHAR; /* NOTE: ARM GCC treats "char" as unsigned */

#ifdef NXT_JSP
#ifndef UINT_DEFINED
#define UINT_DEFINED
typedef unsigned int UINT;
#endif
#else
typedef unsigned  int UINT;
#endif
typedef   signed  int SINT;
typedef         float F32;
typedef        double F64;


typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned long U32;
typedef signed long S32;

typedef struct _Position{
	F64 X;		//ˆÊ’uX[mm]
	F64 Y;		//ˆÊ’uY[mm]
	F64 Ang;	//Šp“x[rad]
} Position;

void getPositionEstimation(const Position& Pre, S32 AngR, S32 AngL, F64 r, F64 d, Position& Now);