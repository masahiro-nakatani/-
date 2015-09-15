#ifndef _INCLUDE_MISSION_H_
#define _INCLUDE_MISSION_H_


#include "Navigator.h"
#include "Robot.h"
#include "Utils.h"

class Mission{
public:

    Mission(void);

    virtual ~Mission(void);

    virtual void Init(RobotInfo ri, NavInfo ni);

    virtual bool Run(RobotInfo, NavInfo, EventFlag, RobotCmd&);

    virtual char* Name(void);

protected:

    void TimerStart(U32 time){
        time_val_ = time;
        time_start_ = clock.now();
        enable_ = true;
    }

    void TimerStop(){
        enable_ = false;
    }

    bool IsTimeup(void){
        if ( enable_ ){
            if ( (clock.now() - time_start_ ) > time_val_ ){
                return true;
            }
        }
        return false;
    }

    bool IsTimerEnable(void){
        return enable_;
    }

    U32 time_val_;
    U32 time_start_;
    bool enable_;

};


#endif
