// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Input/InputHandler.h"

namespace moe
{
	bool IInputHandler::HandleKeyboardEvent(const KeyboardEventDesc& eventDesc)
	{
		bool handled = false;

		for (auto& kbMappingPair : m_keyboardMappings)
		{
			KeyboardMapping& kbMapping = kbMappingPair.second;
			if (kbMapping.Description().Matches(eventDesc))
			{
				kbMapping.Activate();
				handled = true;
			}
		}

		return handled;
	}


	KeyboardMappingID IInputHandler::SetKeyboardMapping(const std::string& mappingName, const KeyboardEventDesc& mappingDesc)
	{
		KeyboardMappingID mappingID;

		auto insertResult = m_keyboardMappings.Emplace(mappingName, KeyboardMapping(m_nextID, mappingDesc));
		MOE_DEBUG_ASSERT(insertResult.first != m_keyboardMappings.End()); // insertResult is a <Iterator, bool> pair
		if (false == insertResult.second) // Our data wasn't inserted : an entry already existed
		{
			// just update the mapping description
			KeyboardMapping& kbMap = insertResult.first->second;
			kbMap.SetDescription(mappingDesc);
			mappingID = kbMap.GetID(); // just return the ID of the existing mapping
		}
		else
		{
			mappingID = m_nextID;
			m_nextID++;
		}

		return mappingID;
	}

}