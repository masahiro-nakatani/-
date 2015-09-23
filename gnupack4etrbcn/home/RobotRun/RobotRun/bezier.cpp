#include <iostream>
#include <sstream>
#include <math.h>

typedef float  F32;
typedef double F64;
typedef int    S32;
typedef short  S16;


class CourseInfo {
public:
	static const F32 ratio;
};

const F32 CourseInfo::ratio = (4770.0/1707.672); // 実寸座標/Inkscape座標.
//const F32 CourseInfo::ratio = (4770.0/1707.672) * (1210.0/1045.127); // 実寸座標/Inkscape座標.

class Vec {
  public:
	Vec (void) : x_(0.0), y_(0.0) { }
	Vec (F32 x, F32 y, bool hosei = true) {
		if (hosei) {
			x_ = x * CourseInfo::ratio;
			y_ = y * CourseInfo::ratio;
		} else {
			x_ = x;
			y_ = y;
		}
	}
	F32 length (void);

	F32 x_, y_;

  private:
};

F32 Vec::length (void) {
	return sqrt(x_ * x_ + y_ * y_);
}

Vec CalcPoint (F32& t, Vec* p) {
	Vec q;
	q.x_ =
		(1.0 - t) * (1.0 - t) * (1.0 - t) * p[0].x_ +
		3.0 * t * (1.0 - t) * (1.0 - t)   * p[1].x_ +
		3.0 * t * t * (1.0 - t)           * p[2].x_ +
		t * t * t                         * p[3].x_;
	q.y_ =
		(1.0 - t) * (1.0 - t) * (1.0 - t) * p[0].y_ +
		3.0 * t * (1.0 - t) * (1.0 - t)   * p[1].y_ +
		3.0 * t * t * (1.0 - t)           * p[2].y_ +
		t * t * t                         * p[3].y_;
		
	return q;
}

Vec CalcDirection (F32& t, Vec* p) {
	Vec dir;
	dir.x_ =
		(-3.0) * (1.0 - t) * (1.0 - t) * p[0].x_ +
		3.0 * (1.0 - t) * (1.0 - t)    * p[1].x_ +
		(-6.0) * (1.0 - t) * t         * p[1].x_ +
		6.0 * (1.0 - t) * t            * p[2].x_ +
		(-3.0) * t * t                 * p[2].x_ +
		3.0 * t * t                    * p[3].x_;
	dir.y_ =
		(-3.0) * (1.0 - t) * (1.0 - t) * p[0].y_ +
		3.0 * (1.0 - t) * (1.0 - t)    * p[1].y_ +
		(-6.0) * (1.0 - t) * t         * p[1].y_ +
		6.0 * (1.0 - t) * t            * p[2].y_ +
		(-3.0) * t * t                 * p[2].y_ +
		3.0 * t * t                    * p[3].y_;
		
	return dir;
}


F32 CalcLength (Vec* p, F32 st, F32 ed, F32 h) {
	// シンプソン則で積分
	S32 nh = (S32)(((ed-st) / h) + 1.0);

	S32 cnt;
	F32 total = 0.0;
	for (cnt = 0; cnt < nh; cnt++) {
		F32 t = h * cnt;
		Vec dir = CalcDirection (t, p);
		F32 tmp = dir.length();
		if (cnt == (nh-1)) {
		} else if (cnt == 0) {
		} else if (cnt % 2 != 0) {
			tmp *= 4.0;
		} else {
			tmp *= 2.0;
		}
		total += tmp;
	}

	return h / 3.0 * total;
}

void createCtrlPoints (const char* ctl, S16& num, Vec*& points)
{
	const char* ptr;
	
	// 制御点の数をチェック
	num = 0;
	for (ptr = ctl; *ptr != '\0'; ++ptr) {
		if (*ptr == 'M') {
			// M means Move-to
			// 最初の制御点
			num++;
		} else if (*ptr == 'C') {
			// C means Curve-to (Cubic Bezier)
			// 制御点3つ
			num += 3;
		} else {
			// Do nothing.
		}
	}

	points = new Vec[num];
	Vec* ctl_ptr = points;
	std::istringstream strin(ctl);
	char cmd;
	char comma;
	F32  x, y;
	while (!strin.eof()) {
		strin >> cmd;
		if (cmd == 'M') {
			strin >> x >> comma >> y;
			*ctl_ptr = Vec(x, y);
			ctl_ptr++;
		} else if (cmd == 'C') {
			strin >> x >> comma >> y;
			*ctl_ptr = Vec(x, y);
			ctl_ptr++;
			strin >> x >> comma >> y;
			*ctl_ptr = Vec(x, y);
			ctl_ptr++;
			strin >> x >> comma >> y;
			*ctl_ptr = Vec(x, y);
			ctl_ptr++;
		} else {
			// Do nothing.
		}
	}
}

void calcAllLength (S16 num, Vec* points, F32*& lengths)
{
	S16 n = (num - 1) / 3;
	lengths = new F32[n];
	for (int i = 0; i < n; i++) {
		lengths[i] = CalcLength (&points[i*3], 0.0, 1.0, 0.00001);
	}
}

void output (S16 num, Vec* points, F32* lengths)
{
	int i;

	printf ("const S16 BezierCourseNum = %d;\n", num);
	printf ("const Vec BezierCourse[] = {\n");
	for (i = 0; i < num; i++) {
		printf ("\tVec(%10.4f, %10.4f),\n", points[i].x_, points[i].y_);
	}
	printf ("};\n\n");
	printf ("const F32 BezierLengths[] = {\n");
	for (i = 0; i < (num-1)/3; i++) {
		printf ("\t%10.4f,\n", lengths[i]);
	}
	printf ("};\n\n");
}

int main (int argc, char** argv)
{
	Vec* points;
	S16  num;
	F32* lengths;

	if (argc < 2) {
		std::cout << "Error! Too few arguments." << std::endl;
		exit (1);
	}
	createCtrlPoints (argv[1], num, points);
	calcAllLength(num, points, lengths);
	output(num, points, lengths);
	
}
