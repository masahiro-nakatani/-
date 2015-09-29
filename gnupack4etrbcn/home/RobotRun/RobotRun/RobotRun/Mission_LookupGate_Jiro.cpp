#if 0
// Žg—p‹ÖŽ~I

#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"


class Mission_LookupGate : public Mission {
public:

    Mission_LookupGate(S32 timer = 0, S16 speed = 125, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission() {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		if (timer_ > 0) {
			timer_start(timer_);
		}
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

        bool ret = false;
		
        return ret;

    }



private:

    TecPIDTrace pid_trace_;
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_LookupGate instance_;

};

//Mission_LookupGate Mission_LookupGate::instance_;
#endif
