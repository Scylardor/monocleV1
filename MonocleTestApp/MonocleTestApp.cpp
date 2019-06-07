// ConsoleApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "Core/Log/moeLog.h"
#include "Windowing/Windows/moeWin32Window.h"
#include "GameFramework/FSM/FSM.h"

//
typedef moe::StdLogger<moe::NoFilterPolicy, moe::NoFormatPolicy, moe::IdeWritePolicy> BasicLogger;

//
//class GameFSM final : public moe::FiniteStateMachine
//{
//public:
//	// The Coplien raindance to make noncopyable data structures compile (because of unique_ptrs)
//	GameFSM() = default;
//	GameFSM(const GameFSM &) = delete;             // copy ctor
//	GameFSM & operator=(const GameFSM &) = delete; // assignment op
//	GameFSM(GameFSM&& other) :
//		moe::FiniteStateMachine(std::move(other)) {}
//	GameFSM& operator=(GameFSM&& other)
//	{
//		moe::FiniteStateMachine::operator=(std::move(other));
//		return *this;
//	}
//	//-----------------------------------------------------------------------------------------
//
//	BasicLogger&	ModifyLogger() { return m_logger; }
//
//	void			Update()
//	{
//		UpdateCurrentState();
//	}
//
//private:
//	BasicLogger m_logger;
//
//};
//
//
//class BonjourState : public moe::IFSMState
//{
//
//public:
//	BonjourState(const std::string& hail = "Bonjour", const std::string& talk = "comment va ?", const std::string& goodbye = "Au revoir") :
//		m_hail(hail), m_talk(talk), m_goodbye(goodbye)
//	{}
//
//
//	virtual void	OnStateEnter(moe::FiniteStateMachine& owner, moe::FSM::StateID thisStateID) override
//	{
//		GameFSM& gameFSM = GetGameFSM(owner);
//
//		MOE_LOGGER_INFO(gameFSM.ModifyLogger(), moe::ChanDefault, "State %i says... '%s'", thisStateID, m_hail);
//	}
//
//	virtual void	OnStateUpdate(moe::FiniteStateMachine& owner, moe::FSM::StateID thisStateID) override
//	{
//		GameFSM& gameFSM = GetGameFSM(owner);
//
//		MOE_LOGGER_INFO(gameFSM.ModifyLogger(), moe::ChanDefault, "State %i says... '%s'", thisStateID, m_talk);
//	}
//
//	virtual void	OnStateExit(moe::FiniteStateMachine& owner, moe::FSM::StateID thisStateID) override
//	{
//		GameFSM& gameFSM = GetGameFSM(owner);
//
//		MOE_LOGGER_INFO(gameFSM.ModifyLogger(), moe::ChanDefault, "State %i says... '%s'", thisStateID, m_goodbye);
//	}
//
//
//	GameFSM&	GetGameFSM(moe::FiniteStateMachine& owner)
//	{
//		return static_cast<GameFSM&>(owner);
//	}
//
//private:
//	std::string m_hail;
//	std::string m_talk;
//	std::string m_goodbye;
//};
//
//
//
//class IdleState : public moe::IFSMState
//{
//	virtual void	OnStateEnter(moe::FiniteStateMachine& , moe::FSM::StateID ) override {}
//	virtual void	OnStateUpdate(moe::FiniteStateMachine& , moe::FSM::StateID ) override {}
//	virtual void	OnStateExit(moe::FiniteStateMachine& , moe::FSM::StateID ) override {}
//};
//
//
//class BonjourTransition : public moe::IFSMTransition
//{
//	virtual bool	Passes(moe::FiniteStateMachine& fsm, moe::IFSMState& stateFrom, moe::FSM::StateID ) override
//	{
//		BonjourState& bonjour = static_cast<BonjourState&>(stateFrom);
//		return (&bonjour.GetGameFSM(fsm) == &fsm);
//	}
//};



int main()
{
	BasicLogger logger;
	MOE_LOGGER_INFO(logger, moe::ChanDefault, "Test app Initialization");

	moe::WindowAttributes winAttr{ moe::Width_t(1024), moe::Height_t(728), L"Test Window" };
	moe::Win32Window window(winAttr);

	moe::ContextDescriptor contextDesc;

	window.CreateContext<moe::D3DContext_11_2>(contextDesc);

	//GameFSM fsm;
	//fsm.AddState<BonjourState>("hola", "que tal", "adios");
	//fsm.AddState<IdleState>();
	//
	//fsm.AddTransition<BonjourTransition>(0, 1);
	//
	//MOE_LOGGER_INFO(logger, moe::ChanDefault, "FSM running ? %d", fsm.IsRunning());
	//
	//fsm.StartState(0);
	//MOE_LOGGER_INFO(logger, moe::ChanDefault, "FSM running ? %d", fsm.IsRunning());
	//fsm.Update();
	//
	//GameFSM fsm2 = std::move(fsm);
	//
	//fsm2.Clear();
	//fsm2.StopFSM();
	//
	//
	//test t;

	moe::FSM fsm;

	bool keepGoing = true;
	while (keepGoing)
	{
		keepGoing = window.ProcessWindowEvents();
	}

    return 0;
}

