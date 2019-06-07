// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <Windows.h> // HWND

#include "Input/RawInputHandler.h"

namespace moe
{
	/*
		Win32 raw input handler
		Will listen for keyboards, mice.
		Additional features may come later.
	*/
	class MOE_DLL_API Win32RawInputHandler : public IRawInputHandler<Win32RawInputHandler>
	{
	public:

		typedef HWND	WindowHandle;

		bool	BindToWindowRawInputDevices(WindowHandle winHandle);


		bool	UnregisterRawInputDevices()
		{
			return true;
		}

	};

}