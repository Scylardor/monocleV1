// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
	/*
		RAII class to deal with Raw Input (keyboard and mice).
		Put all the platform-specific code inside children classes.
	*/

	template <class DerivedT>
	class MOE_DLL_API IRawInputHandler
	{
	public:

		typedef DerivedT::WindowHandle	WindowHandle;



		virtual ~IRawInputHandler() = default;


		bool	BindToWindowRawInputDevices(WindowHandle winHandle)
		{
			return Derived().BindToWindowRawInputDevices();
		}


		bool	UnregisterRawInputDevices()
		{
			return Derived().BindToWindowRawInputDevices();
		}



	private:
		DerivedT&	Derived()
		{
			return static_cast<DerivedT&>(*this);
		}

		const DerivedT&	Derived() const
		{
			return static_cast<const DerivedT&>(*this);
		}
	};

}