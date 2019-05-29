// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint> // std::uint8_t
#include "Core/Misc/moeEnum.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Delegates/Event.h"

namespace moe
{

	typedef std::uint8_t	Keycode;
	typedef std::uint32_t	KeyboardMappingID;
	typedef moe::Event<void(KeyboardMappingID)>		KeyboardMappingEvent;
	typedef moe::Delegate<void(KeyboardMappingID)>	KeyboardMappingDelegate;


	MOE_ENUM(KeyState, std::uint8_t,
		Pressed = 0,
		Repeated,
		Released
	);
	DECLARE_MOE_ENUM_OPERATORS(KeyState);


	/*
		Describes a keyboard event:
		- a keycode
		- whether special keys like alt, shift or control must be down too
		- whether it's meant to be fired on key pressed, repeated, or released, or any combination of the three
	*/
	struct KeyboardEventDesc
	{
		KeyboardEventDesc(Keycode code, KeyState state, bool shift = false, bool alt = false, bool ctrl = false) :
			m_code(code), m_state(state), m_shift(shift), m_alt(alt), m_ctrl(ctrl)
		{}

		/*
			Note that Matches is "not commutative":
			for example, a keyboard description with Shift false will match the same keyboard description with Shift set to true.
			But if it's the other way around, a keyboard description with shift set to True won't match a description where Shift is set to false.
		*/
		bool	Matches(const KeyboardEventDesc& desc) const
		{
			bool matches = (
				m_code == desc.m_code
				&& (m_state & desc.m_state)
				&& (desc.m_shift >= m_shift)
				&& (desc.m_alt >= m_alt)
				&& (desc.m_ctrl >= m_ctrl)
				);
			return matches;
		}

		Keycode		m_code;
		bool		m_shift = false;
		bool		m_alt = false;
		bool		m_ctrl = false;
		KeyState	m_state;
	};


	/*
		Declares a mapping to a keyboard event
	*/
	class KeyboardMapping
	{
	public:

		KeyboardMapping(KeyboardMappingID mapID, const KeyboardEventDesc& mapDesc) :
			m_id(mapID), m_mappingDesc(mapDesc)
		{}


		KeyboardMappingEvent&	ModifyMappingEvent()
		{
			return m_event;
		}

		void	SetDescription(const KeyboardEventDesc& mapDesc)
		{
			m_mappingDesc = mapDesc;
		}

		void	Activate()
		{
			m_event.Broadcast(m_id);
		}


		const KeyboardEventDesc&	Description() const
		{
			return m_mappingDesc;
		}


		KeyboardMappingID	GetID() const { return m_id; }

	private:
		KeyboardMappingID	m_id;
		KeyboardEventDesc	m_mappingDesc;


		KeyboardMappingEvent	m_event;
	};


}