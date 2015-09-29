#ifndef __VEC_H_INCLUDED__
#define __VEC_H_INCLUDED__

#include "ecrobot_types.h"
#include <math.h>

#include "CourseInfo.h"

class Vec {
  public:
	Vec (void) : x_(0.0), y_(0.0) { }
	Vec (F32 x, F32 y) : x_(x), y_(y) { }
	Vec (F32 x, F32 y, bool hosei) {
		if (hosei) {
			x_ = x * CourseInfo::ratio;
			y_ = y * CourseInfo::ratio;
		} else {
			x_ = x;
			y_ = y;
		}
	}

	Vec add (Vec& v);
	Vec sub (Vec& v);
	F32 dot (Vec& v);
	F32 cross (Vec& v);
	F32 length (void);
	Vec unit (void);

	F32 x_, y_;

  private:
};


#endif
