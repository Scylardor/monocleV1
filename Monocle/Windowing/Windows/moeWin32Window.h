#ifndef MOE_WIN32_WINDOW_H_
#define MOE_WIN32_WINDOW_H_

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Windowing/moeWindow.h"
#include "Graphics/Context/Windows/moeWGLContext.h"
#include "Graphics/Context/Windows/moeD3DContext_11_1.h"
#include "Graphics/Context/Windows/moeD3DContext_11_2.h"
#include <windows.h> // HWND

namespace moe
{
	// needed in order to specialize WindowTraits with our not-yet-defined Win32Window
	// We *must* define WindowTraits BEFORE the actual window class because
	// WindowBase inheritance will need traits to statically enforce window's properties.
	class Win32Window;

	template<>
	struct WindowTraits<Win32Window>
	{
		using HandleType = HWND;
		using CompatibleContexts = moe::TypeList<moe::WGLContext, moe::D3DContext_11_1, moe::D3DContext_11_2>;
	};

	class Win32Window : public WindowBase<Win32Window>
	{
	public:
		MOE_DLL_API Win32Window(const WindowAttributes& winAttr);
		MOE_DLL_API ~Win32Window();

		/* Returns whether the window should quit (received WM_QUIT). */
		bool    MOE_DLL_API ProcessWindowEvents();

		bool    Initialized() const;

	private:
		// WindowBase needs to be friend because it's gonna call CreateConcreteContext
		friend WindowBase<Win32Window>;

		template <class Context>
		void	CreateConcreteContext(const ContextDescriptor& contextDesc);

		bool    InitializeWindow(const WindowAttributes& winAttr);

		bool    RegisterWindow(const WindowAttributes& winAttr);

		bool    CreateWindowHandle(const WindowAttributes& winAttr);

		LRESULT	ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void    DestroyWindow();

		void    DestroyWindowHandle();

		void	TranslateRawInputButtonFlags(USHORT buttonFlags);


		Vector<BYTE> m_rawInputBuffer; /* This is a byte buffer used to retrieve RAWINPUT information. keeping it a member variable keeps from allocating memory everytime */

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static const wchar_t*  WINDOW_CLASS;

		static const int	s_expectedMaxRawInputBufferSize = 48; // we do not expect rawinput buffers to exceed this size in bytes (see http://keithditch.powweb.com/Games/html/raw_input.html )
	};

	template <>
	MOE_DLL_API void    Win32Window::CreateConcreteContext<moe::WGLContext>(const ContextDescriptor& contextDesc);

	template <>
	MOE_DLL_API void    Win32Window::CreateConcreteContext<moe::D3DContext_11_2>(const ContextDescriptor& contextDesc);

	template <>
	MOE_DLL_API void    Win32Window::CreateConcreteContext<moe::D3DContext_11_1>(const ContextDescriptor& contextDesc);

}

#endif // MOE_WIN32_WINDOW_H_
