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

	Vec add (const Vec& v) const;
	Vec sub (const Vec& v) const;
	F32 dot (const Vec& v) const;
	F32 cross (const Vec& v) const;
	F32 length (void) const;
	Vec unit (void) const;

	F32 x_, y_;

  private:
};


#endif
