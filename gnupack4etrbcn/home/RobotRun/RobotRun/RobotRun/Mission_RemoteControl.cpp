#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"
#include "GamePad.h"
class Mission_RemoteControl : public Mission {
public:

    Mission_RemoteControl(Bluetooth* pBT, S32 timer = 0, S16 speed = 125, S16 tail = RobotCmd::NO_TAIL_CNTL) : Mission(),pBT_( pBT ) {
		timer_ = timer;
		speed_ = speed;
		tail_  = tail;
    }

    virtual void Init(RobotInfo ri, NavInfo ni){
        pid_trace_.Init();
		if (timer_ > 0) {
			timer_start(timer_);
		}
		//Navigator::Instance().StartDirCorrection (400.0, 180.0);
    }

	enum{
		receive_data_size = 6
	};

    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){
		U8 data[receive_data_size] = {0}; // Speed, Left, Right
		
		volatile U32 size = 0;
		if (!pBT_->isConnected()) return false;

		timer_start( 1000 );

		while( 1)
		{
			Sleep_msec(10);

			size += pBT_->receive( data, size, sizeof(data) );

			if (size >= receive_data_size || is_timeup())
			{
				break;
			}
		}
		if (size)
		{
			Robot::Instance().GetLCD().clear();
			Robot::Instance().GetLCD().putf("sds", "SIZE:", size,0, "DATA:");
			for(size_t i = 0; i < sizeof(data); i++)
				Robot::Instance().GetLCD().putf("d",(U32)data[i],0);
			

			cmd.param1 = data[0] > 0 ? static_cast<S16>(data[1]) : static_cast<S16>(-1 * data[1]);
			cmd.param2 = data[2] > 0 ? static_cast<S16>(data[3]) : static_cast<S16>(-1 * data[3]);
			cmd.param3 = data[4];
			cmd.Mode = data[5] > 0 ? RobotCmd::NORMAL_MODE : RobotCmd::DIRECT_MODE;

			Robot::Instance().GetLCD().putf("\nsddd", "Command:", cmd.param1,0, cmd.param2,0, cmd.param3,0);

			Robot::Instance().GetLCD().disp();

			return true;
		}

		Robot::Instance().GetLCD().clear();
		Robot::Instance().GetLCD().putf("s", "Failed");
		Robot::Instance().GetLCD().disp();

		cmd.param1 = 0;
		cmd.param2 = 0;
		cmd.Mode = RobotCmd::DIRECT_MODE;
		
		return true;

    }



private:
	Bluetooth* pBT_;
    TecPIDTrace pid_trace_;
	S32 timer_;
	S16 speed_;
	S16 tail_;

//    static Mission_Default instance_;

};
