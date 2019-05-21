// Monocle Game Engine source files - Alexandre Baron
//
#include "FSM/FSM.h"
#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
	bool FSM::HasState(FSM::StateID id) const
	{
		return (mStateData.Find((int)id) != mStateData.End());
	}

	void FSM::StartState(StateID id)
	{
		if (m_currentStateID != s_UninitializedState)
		{
			StateDataTest& exitingStateData = ModifyStateData(m_currentStateID);
			exitingStateData.m_state->OnStateExit(*this, m_currentStateID);
		}

		StateDataTest& enteringStateData = ModifyStateData(id);
		enteringStateData.m_state->OnStateEnter(*this, id);
		m_currentStateID = id;
	}


	void FSM::RemoveState(StateID removedId)
	{
		MOE_DEBUG_ASSERT(HasState(removedId));

		// First remove the given state from the states hash map
		mStateData.Erase(removedId);

		// Then look for transitions pointing to this state in all other states
		for (auto& fsmDataIt : mStateData)
		{
			StateDataTest& stateData = fsmDataIt.second;
			StateDataTest::StateTransitionsData& transData = stateData.mTrData;
			if (false == stateData.mTrData.Empty())
			{
				transData.Erase(
					std::remove_if(transData.Begin(), transData.End(), [removedId](const auto& td) { return td.m_destination == removedId; }),
					transData.End()
				);
			}
		}
	}


	FSM::TransitionNumber FSM::RemoveStateTransition(FSM::StateID stateId, FSM::TransitionID transId)
	{
		MOE_DEBUG_ASSERT(HasState(stateId));
		StateDataTest::StateTransitionsData& stateTransData = ModifyStateData(stateId).mTrData;

		MOE_DEBUG_ASSERT(stateTransData.Size() > transId);

		stateTransData.EraseBySwapAt(transId);

		return stateTransData.Size();
	}


	FSM::TransitionNumber FSM::GetStateNumberOfTransitions(StateID id) const
	{
		MOE_DEBUG_ASSERT(HasState(id));
		return GetStateData(id).mTrData.Size();
	}


	void FSM::StopFSM()
	{
		if (IsRunning())
		{
			ModifyStateData(m_currentStateID).m_state->OnStateExit(*this, m_currentStateID);
			m_currentStateID = s_UninitializedState;
		}
	}


	void FSM::Clear()
	{
		mStateData.Clear();
		m_currentStateID = s_UninitializedState;
		m_stateNbr = 0;
		m_transitionNbr = 0;
	}

}

