// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/HashString/HashString.h"

#include "Input/KeyboardMapping.h"

#include <string>

namespace moe
{
	class MOE_DLL_API IInputHandler
	{
	public:

		typedef moe::Delegate<void(InputMappingID)>	ActionMappingDelegate;


	private:
		typedef moe::Event<void(InputMappingID)>		ActionMappingEvent;

		struct	ActionDescriptor
		{
			moe::Vector<KeyboardMapping>	m_kbMaps;

			ActionMappingEvent				m_actionEvent;

			void	Activate(InputMappingID activatedMappingId)
			{
				m_actionEvent.Broadcast(activatedMappingId);
			}
		};

		typedef	HashMap<HashString, ActionDescriptor>	ActionMappings;


	public:

		/*
			Maps a keyboard event to the given action mapping. Creates a new one if it doesn't exist yet.
			Returns a keyboard mapping ID useful to identify this mapping.
		*/
		InputMappingID	BindActionMappingKeyboardEvent(const HashString& mappingName, const KeyboardEventDesc& mappingDesc);



		void		HandleKeyDown(Keycode key);
		void		HandleKeyUp(Keycode key);


		void		AddActionMappingDelegate(const HashString& mappingName, const ActionMappingDelegate& dlgt);
		void		AddActionMappingDelegate(const HashString& mappingName, ActionMappingDelegate&& dlgt);


		bool		HasActionMapping(const HashString& mappingName) const;

		// Activate the action mapping externally. Will pass the action mapping ID as parameter to listener delegates.
		bool		ActivateActionMapping(const HashString& mappingName);


		ActionMappings::SizeType	NumActionMappings() const
		{
			return m_actionMappings.Size();
		}


	private:

		bool IInputHandler::HandleKeyboardEvent(const KeyboardEventDesc& eventDesc);


		InputMappingID	m_nextID = 0;
		ActionMappings	m_actionMappings;

	};
}