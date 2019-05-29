// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Input/KeyboardMapping.h"

#include <string>

namespace moe
{
	class MOE_DLL_API IInputHandler
	{
		typedef	std::uint32_t	MappingID;
		typedef	HashMap<std::string, KeyboardMapping>	KeyboardMappings;


		bool	HandleKeyboardEvent(const KeyboardEventDesc& eventDesc);


		/*
			Returns a keyboard mapping ID useful to identify the activated mapping in a mapping callback.
			This way, you can for example bind the same function to multiple mappings and still be able to tell
			which mapping has been activated.
		*/
		KeyboardMappingID	SetKeyboardMapping(const std::string& mappingName, const KeyboardEventDesc& mappingDesc);

	private:
		MappingID			m_nextID = 0;
		KeyboardMappings	m_keyboardMappings;

	};
}