#include "Mission.h"
#include "Technic.h"
#include "Robot.h"
#include "debug.h"
#include "Navigator.h"
#include "CourseInfo.h"

#define MAX_SPEED 110
#define S_SPEED 100
#define MIN_SPEED 60

namespace
{
	class ActionFacade;
}

namespace 
{
	class IAction
	{
	public:
		virtual ~IAction(){}
		IAction(){}
		virtual RobotCmd DoAction(const RobotInfo& ri, const NavInfo& ni) = 0;
	};

	class IActionContinuable
	{
	public:
		virtual ~IActionContinuable(){}
		virtual bool IsContinuable(const RobotInfo& ri, const NavInfo& ni, EventFlag evf) const = 0;
		virtual void UpdateCurrentState(const NavInfo& ni, EventFlag evf) = 0;
	};

	class INotifier
	{
	public:
		virtual ~INotifier(){}
		virtual void Notify() = 0;
	};

	class IActionValidator
	{
	public:
		virtual ~IActionValidator(){}
		virtual bool Validate(const RobotInfo& ri, const NavInfo& ni, EventFlag evf) = 0;
		virtual void UpdateCurrentState(const NavInfo& ni, EventFlag evf) = 0;
		virtual const char* Why() const = 0;
	};

	class ActionFacade
	{
		IAction* p_action_;
		IActionContinuable* p_action_continuable_;
		IActionValidator* p_action_validator_; 
		INotifier* p_action_notifier_;

		ActionFacade(const ActionFacade&);
		ActionFacade& operator=(const ActionFacade&);
	public:
		ActionFacade(
			IAction* p_action, 
			IActionContinuable* p_action_continuable, 
			IActionValidator* p_action_validator,
			INotifier* p_action_notifier)
			: p_action_(p_action)
			, p_action_continuable_(p_action_continuable)
			, p_action_validator_(p_action_validator)
			, p_action_notifier_(p_action_notifier)
		{}
		~ActionFacade()
		{
			delete p_action_;
			delete p_action_continuable_;
			delete p_action_validator_;
			delete p_action_notifier_;
		}
		bool IsContinuable(const RobotInfo& ri, const NavInfo& ni, EventFlag evf) const
		{
			return p_action_continuable_->IsContinuable(ri, ni,evf);
		}
		bool ValidateParam(const RobotInfo& ri, const NavInfo& ni, EventFlag evf)
		{
			if(p_action_validator_==0) return true;
			return p_action_validator_->Validate(ri,ni,evf);
		}
		void NotifyActionFinished() const
		{
			if(p_action_notifier_!=0) p_action_notifier_->Notify();
		}
		RobotCmd DoAction(const RobotInfo& ri, const NavInfo& ni)
		{
			return p_action_->DoAction(ri, ni);
		}
		const char* Why() const
		{
			return p_action_validator_->Why();
		}
		void UpdateCurrentState(const NavInfo& ni, EventFlag evf)
		{
			p_action_continuable_->UpdateCurrentState(ni,evf);
			p_action_validator_->UpdateCurrentState(ni,evf);
		}
	};

	ActionFacade* CreateAction(
		IAction* p_action, 
		IActionContinuable* p_action_continuable, 
		IActionValidator* p_action_validator,
		INotifier* p_action_notifier)
	{
		return new  ActionFacade(
			p_action,
			p_action_continuable,
			p_action_validator,
			p_action_notifier );
	}

	ActionFacade* CreateActionWithoutNotification(
		IAction* p_action, 
		IActionContinuable* p_action_continuable, 
		IActionValidator* p_action_validator)
	{
		return new  ActionFacade(
			p_action,
			p_action_continuable,
			p_action_validator,
			0 );
	}

	ActionFacade* CreateActionWithoutValidation(
		IAction* p_action, 
		IActionContinuable* p_action_continuable, 
		INotifier* p_action_notifier)
	{
		return new  ActionFacade(
			p_action,
			p_action_continuable,
			0,
			p_action_notifier );
	}
}

namespace
{
	class PIDTrace : public IAction
	{
		TecPIDTrace pid_trace_;
		Technic::DIRECTION edge_;
		S16 speed_;
		S16 tail_;
	public:
		PIDTrace(Technic::DIRECTION edge, S16 speed, S16 tail)
			: pid_trace_()
			, edge_(edge)
			, speed_(speed)
			, tail_(tail)
		{
		}
		virtual RobotCmd DoAction(const RobotInfo& ri, const NavInfo& ni) 
		{
			RobotCmd cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = tail_;
			return RobotCmd(cmd);
		}
	};

	class SoundBeepWhenFinished : public INotifier
	{
	public:
		virtual void Notify()
		{
			Robot::Instance().Beep();
		}
	};

	class MustBeDoneUntilTimeup : public IActionValidator
	{
		U32 time_val_;
		U32 time_start_;
		char msg_[255];
	public:
		MustBeDoneUntilTimeup(U32 time)
			: time_val_(time)
			, time_start_(clock.now())
		{}
		virtual bool Validate(const RobotInfo& ri, const NavInfo& ni, EventFlag evf)
		{
			return (clock.now() - time_start_ ) > time_val_;
		}
		virtual const char* Why() const 
		{
			return "time up";
		}
		virtual void UpdateCurrentState(const NavInfo& ni, EventFlag evf)
		{
			time_start_=clock.now();
		}

	};

	class DoUntilTimeup : public IActionContinuable
	{
		U32 time_val_;
		U32 time_start_;
		bool init_;
	public:
		DoUntilTimeup(U32 time)
			: time_val_(time)
			, time_start_(clock.now())
			, init_(false)
		{}
		virtual bool IsContinuable(const RobotInfo& ri, const NavInfo& ni, EventFlag evf) const 
		{
			if(!init_)
			{
				return true;
			}
			return !( (clock.now() - time_start_ ) > time_val_);
		}
		virtual void UpdateCurrentState(const NavInfo& ni, EventFlag evf)
		{
			time_start_=clock.now();
			init_=true;
		}
	};

	class DoUntilSpecifiedDistancePassed : public IActionContinuable
	{
		F64 x_;
		F64 y_;
		F64 start_x_;
		F64 start_y_;
	public:
		DoUntilSpecifiedDistancePassed(F64 x, F64 y)
			: x_(x)
			, y_(y)
			, start_x_(0.0)
			, start_y_(0.0)
		{}
		virtual bool IsContinuable(const RobotInfo& ri, const NavInfo& ni, EventFlag evf) const 
		{
			return (ni.pos.X - start_x_) >x_ && (ni.pos.Y - start_y_) > y_;
		}
		virtual void UpdateCurrentState(const NavInfo& ni, EventFlag evf)
		{
			start_x_=ni.pos.X;
			start_y_=ni.pos.Y;
		}
	};
}


class Mission_Timed_PID : public Mission {
	
	enum
	{
		TAIL_DOWN = 0
	};
	U32 time_val_;
	U32 time_start_;
	TecPIDTrace pid_trace_;
	Technic::DIRECTION edge_;
	S16 speed_;
	S16 tail_;
public:

    Mission_Timed_PID(S16 speed, S16 tail, U32 time, Technic::DIRECTION edge = Technic::LEFT) 
		: time_val_(time)
		, time_start_(clock.now())
		, edge_(edge)
		, speed_(speed)
		, tail_(tail)
	{
	}

    virtual void Init(RobotInfo ri, NavInfo ni)
	{
		time_start_=clock.now();
    }


    virtual bool Run(RobotInfo ri, NavInfo ni, EventFlag evf, RobotCmd& cmd ){

		if( (clock.now() - time_start_ ) > time_val_ )
		{
			RobotCmd cmd = pid_trace_.Calclate(ri.light_sensor_val, speed_, edge_);
			cmd.param3 = tail_;
			return true;
		}

		Robot::Instance().Beep();

		return false;
    }
};
