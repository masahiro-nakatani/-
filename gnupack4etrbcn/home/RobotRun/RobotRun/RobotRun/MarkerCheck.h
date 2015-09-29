#ifndef __MARKER_CHECK_H__
#define __MARKER_CHECK_H__

#include "ecrobot_types.h"
#include "jiro_type.h"
#include "Navigator.h"

#define BRTLOGNUM		10		//200ms÷20ms=10
#define ANGLOGNUM		10		//200ms÷20ms=10

class MarkerCheck
{

//メソッド
//　マーカー検知する。
public:
	MarkerCheck()
		: count (0)
		, grayProbability (0)
		, countMax (3)
	{ }
	
	int IsMarker(S16 bright);

private:
	bool is_straight();
	
	int count;
	int grayProbability;
	int countMax;
	S16 brightLog[BRTLOGNUM];
	F32 angLog[ANGLOGNUM];
};



#endif /* __MARKER_CHECK_H__*/
