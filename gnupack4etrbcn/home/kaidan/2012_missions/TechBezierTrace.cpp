#include "Technic.h"
#include "Utils.h"
#include "Debug.h"

void TecBezierTrace::Init (NavInfo ni, const Vec* ctl, const S32* time, const S32 num, bool isStopMode) {
	org_pos_ = ni.pos;
	org_time_ = clock.now();
	ctl_point_ = ctl;
	ctl_time_ = time;
	ctl_point_num_ = num;
	isFinished_ = false;
	isStopMode_ = isStopMode;
}

RobotCmd TecBezierTrace::Calclate (Position& pos) {
	S32 nowTime = clock.now();
	F32 t = CalcParamT (nowTime);
	if (t > 1.0) {
		if (AlterCtlPoint(nowTime)) {
            // ����_���ύX���ꂽ���߁A�p�����[�^t���Čv�Z
			t = CalcParamT (nowTime);
		} else if (isStopMode_) {
			t = 1.001;
		}
	}
	Vec targetPos = CalcPoint (t);
	Vec selfPos   = Vec(pos.X - org_pos_.X, pos.Y - org_pos_.Y);
	Vec v = targetPos.sub(selfPos);
	F32 len = v.length();                  // ����
	Vec targetDir = v.unit();
	Vec selfDir   = Vec(cos(-(pos.Ang - org_pos_.Ang)), sin(-(pos.Ang - org_pos_.Ang))); // ���{�b�g�̕���
	F32 dot   = selfDir.dot (targetDir);   // cos
	F32 cross = selfDir.cross (targetDir); // sin
	S16 speed = (S16)(
		(100.0 - 0.0) /           // ���i�� (��� - ����) [%]
		(200.0  - 0.0) *           // ����   (��� - ����) [mm]
		(len - 0.0) + 0.0);
	if (speed > 100) speed = 100;
	if (dot < 0.0) {
		speed = -1 * speed;
	}
	S16 turn  = (S16)(
		(50.0 - (-50.0)) /            // ��]�� (��� - ����) [%]
		( 1.0 - ( -1.0)) *            // sin    (��� - ����) [0, 1]
		(cross - ( -1.0)) + (-50.0));
#if 0
	Robot::Instance().GetLCD().cursor(0,2);
	Robot::Instance().GetLCD().putf("sd", "t :", (S32)(t*10000), 12);
	Robot::Instance().GetLCD().cursor(0,3);
	Robot::Instance().GetLCD().putf("sdd","tP:", (S32)targetPos.x_, 6, (S32)targetPos.y_, 6);
	Robot::Instance().GetLCD().cursor(0,4);
	Robot::Instance().GetLCD().putf("sdd","mP:", (S32)selfPos.x_, 6, (S32)selfPos.y_, 6);
	Robot::Instance().GetLCD().cursor(0,5);
	Robot::Instance().GetLCD().putf("sd", "ln:", (S32)len, 12);
	Robot::Instance().GetLCD().cursor(0,6);
	Robot::Instance().GetLCD().putf("sdd", "Ag:", (S32)(atan2(selfDir.y_,selfDir.x_)*180.0/M_PI), 6, (S32)(asin(cross)*180.0/M_PI), 6);
	Robot::Instance().GetLCD().cursor(0,7);
	Robot::Instance().GetLCD().putf("sdd", "Cm:", speed, 6, turn, 6);
	Robot::Instance().GetLCD().disp();
#endif
	//if (speed > 100) speed = 100;
	if (t > 1.0 && len < 50.0) {
		// t��1�𒴂��Ă��āA������5cm�����Ȃ�I�����
		isFinished_ = true;
	}
	return RobotCmd (speed, turn);
}

bool TecBezierTrace::isLastCtlPoints () {
	if (ctl_point_num_ > 3) {
		return false;
	}
	// ����_����3�ȉ��̏ꍇ�A�Ō�̐���_��Ƃ���
	return true;
}

bool TecBezierTrace::isFinished () {
	return isFinished_;
}

F32 TecBezierTrace::CalcParamT (S32& nowTime) {
	return ((F32)nowTime - org_time_) / *ctl_time_;
}

bool TecBezierTrace::AlterCtlPoint (S32& nowTime) {
	if (!isLastCtlPoints()) {
		org_time_ = nowTime;
		// 2����Bezier�Ȑ��ł��邽��2�����炷
		ctl_point_     += 2;
		ctl_point_num_ -= 2;
		ctl_time_      += 1;
		return true;
	}

	// �Ō�̐���_��̏ꍇ
	return false;
}

Vec TecBezierTrace::CalcPoint (F32& t) {
	Vec p;
	p.x_ =
		(1.0 - t) * (1.0 - t) * ctl_point_[0].x_ +
		2.0 * t * (1.0 - t) * ctl_point_[1].x_ +
		t * t * ctl_point_[2].x_;
	p.y_ =
		((1.0 - t) * (1.0 - t) * ctl_point_[0].y_ +
		 2.0 * t * (1.0 - t) * ctl_point_[1].y_ +
		 t * t * ctl_point_[2].y_);
	return p;
}

Vec TecBezierTrace::CalcDirection (F32& t) {
	Vec dir;
	dir.x_ =
		(t - 1.0) * ctl_point_[0].x_ +
		(1.0 - 2.0 * t) * ctl_point_[1].x_ +
		t * ctl_point_[2].x_;
	dir.y_ =
		(t - 1.0) * ctl_point_[0].y_ +
		(1.0 - 2.0 * t) * ctl_point_[1].y_ +
		t * ctl_point_[2].y_;
	return dir;
}
