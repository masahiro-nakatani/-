#include "vec.h"

Vec Vec::add (const Vec& v) const{
	return Vec(x_ + v.x_, y_ + v.y_);
}
	
Vec Vec::sub (const Vec& v) const{
	return Vec(x_ - v.x_, y_ - v.y_);
}

F32 Vec::dot (const Vec& v) const{
	return x_ * v.x_ + y_ * v.y_;
}

F32 Vec::cross (const Vec& v) const{
	return x_ * v.y_ - y_ * v.x_;
}

F32 Vec::length (void) const{
	return sqrt(x_ * x_ + y_ * y_);
}

Vec Vec::unit (void) const{
	F32 len = length();
	if (len == 0.0) {
		return Vec(0.0, 0.0);
	}
	return Vec(x_ / len, y_ / len);
}

