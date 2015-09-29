#ifndef _INCLUDE_UTILS_H_
#define _INCLUDE_UTILS_H_

#include "Clock.h"
#include "kernel.h"

#include <string.h>

void Sleep_msec(U32);

using namespace ecrobot;
extern Clock clock;

class Lock {
public:
    Lock(ResourceType rid) : resid_(rid){
        GetResource(resid_);
    }
    virtual ~Lock(void){
        ReleaseResource(resid_);
    }

    void lock(void){
        GetResource(resid_);
    }

    void unlock(void){
        ReleaseResource(resid_);
    }

private:

    ResourceType resid_;

};

//**************************************************
// イベントフラグクラス（最大１６イベント（ID：０～１５）まで管理）
//**************************************************
#define ID_MAX 16
class EventFlag {
public:
    EventFlag(void){
        this->Clear();
    }
    ~EventFlag(void){
    }

    void Clear(void){
        ::memset(flags_, 0, sizeof(flags_) );
    }

    void Set(int eid){
        if ( eid < ID_MAX ){
            this->flags_[eid] = true;
        }
    }

    bool is_SetEvent(int eid){
        return this->flags_[eid];
    }


private:
    // hidden

    bool flags_[ID_MAX];

};




#endif
