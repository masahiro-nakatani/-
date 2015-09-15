#include "Clock.h"
#include "Utils.h"


using namespace ecrobot;
Clock clock;

void Sleep_msec(U32 t){
    clock.sleep(t);
}
