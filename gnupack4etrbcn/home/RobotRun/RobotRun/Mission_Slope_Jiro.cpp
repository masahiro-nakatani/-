#if 0
// égópã÷é~!

#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"
#include "BezierCourseInfo.h"

#define DIST_STRAIGHT_BEFORE	250000//1061317
#define DIST_UP_SLOPE			361544.2
#define DIST_DOWN_SLOPE			1382616



class Mission_Slope : public Mission {
private:
	enum LocalState {
		P_START,
		P_STRAIGHT_BEFORE,
		P_UP_SLOPE,
		P_DOWN_SLOPE,
		P_STRAIGHT_AFTER,
	};
	

public:

    Mission_Slope() : Mission(){
	}
		
    virtual void Init(RobotInfo ri, NavInfo ni){
		speed_ = 100;
		bezier_.Init (ni, BezierCourse, BezierLengths, BezierCourseNum, speed_, false);
    	local_status_ = P_START;
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = true;

		if (P_START == local_status_) {
			cmd = bezier_.Calclate(ni.pos);
			cmd.param3 = 10;
			if (bezier_.isFinished()) {
				Robot::Instance().Beep();
				// èÛë‘ëJà⁄
				ret = false;
			}
		}

        return ret;

    }



private:

	S16 speed_;
    TecBezierTrace bezier_;
	LocalState local_status_;
};


#endif

