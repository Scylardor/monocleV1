// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint> // std::uint8_t
#include "Core/Misc/moeEnum.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Delegates/Event.h"

#include "Input/Input.h"
#include "Input/MonocleKeyboardMap.h"


namespace moe
{

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
		KeyboardEventDesc(Keycode code, KeyState state) :
			m_code(code), m_state(state)
		{}

		/*
			Note that Matches is "not commutative":
			for example, a keyboard description with Shift false will match the same keyboard description with Shift set to true.
			But if it's the other way around, a keyboard description with shift set to True won't match a description where Shift is set to false.
		*/
		bool	Matches(const KeyboardEventDesc& desc) const
		{
			bool matches = (m_code == desc.m_code && (m_state & desc.m_state));
			return matches;
		}

		Keycode		m_code;
		KeyState	m_state;
	};


	/*
		Declares a mapping to a keyboard event
	*/
	class KeyboardMapping
	{
	public:

		KeyboardMapping(InputMappingID mapID, const KeyboardEventDesc& mapDesc) :
			m_id(mapID), m_mappingDesc(mapDesc)
		{}



		void	SetDescription(const KeyboardEventDesc& mapDesc)
		{
			m_mappingDesc = mapDesc;
		}

		const KeyboardEventDesc&	Description() const
		{
			return m_mappingDesc;
		}


		InputMappingID	GetID() const { return m_id; }

	private:
		InputMappingID	m_id;
		KeyboardEventDesc	m_mappingDesc;
	};


}