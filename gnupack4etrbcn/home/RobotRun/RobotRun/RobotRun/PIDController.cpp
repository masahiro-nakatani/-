#include "PIDController.h"

PIDParam pid_light_param     = { 1.0, 0.0, 30.0 };
//PIDParam pid_direction_param = { 0.7, 0.1, 0.3 };

void pid_calc_rotation (F32 kp, F32 ki, F32 kd, F32 e0, F32 e1, F32 e2, F32* rot) {
	*rot
		= kp * (e0 - e1)
		+ ki * e0
		+ kd * ((e0 - e1) - (e1 - e2));
}

void pid_calc_rotation (PIDParam* p, F32 e0, F32 e1, F32 e2, F32* rot) {
	pid_calc_rotation (p->kp, p->ki, p->kd, e0, e1, e2, rot);
}
