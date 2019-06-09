#ifndef MOE_WINDOW_H_
#define MOE_WINDOW_H_

#include "Core/Misc/moeTypeList.h"
#include "Core/Misc/moeFalse.h"
#include "Graphics/Context/moeGraphicsContext.h"
#include "Input/InputHandler.h"

#include "moeWindowAttributes.h"

#include <memory> // unique_ptr

// moe::WindowBase is a CRTP class used to instantiate platform-specific windows.
// It is the core of Monocle windowing system, and is based on the assumption that
// on a given OS, we can only build one type of window (e.g. Win32 on Windows, or X11
// on Linux; you can't e.g. create X11 windows on... Windows.
// So the advantage of this assumption is that windows type can be known at compile-time.
// This may not be so true with e.g. Wayland on Linux, and may need to be adapted in the future.
namespace moe
{
	// To use a custom window with moe::WindowBase, you'll need to provide traits for it.
	// Required traits:
	// - HandleType: the type of the native handle of the window (e.g. HWND for Win32 windows)
	// - CompatibleContexts: a moe::TypeList of contexts that can be used with that window
	template <typename T>
	struct WindowTraits
	{
		using HandleType = void;
		using CompatibleContexts = moe::TypeList<moe::NullType>;
		static_assert(False<T>::value, "Define a WindowTraits specialization for your Window type to be able to inherit WindowBase");
	};



	template <class ConcreteWindow>
	class WindowBase
	{
	public:
		using Handle = typename WindowTraits<ConcreteWindow>::HandleType;

		WindowBase(const WindowAttributes& attribs) :
			m_attribs(attribs)
		{
		}


		bool    InitializeWindow(const WindowAttributes& winAttr)
		{

			return window().InitializeWindow(winAttr);
		}


		/* Returns whether the window should quit (received a QUIT signal). */
		bool    ProcessWindowEvents()
		{
			return window().ProcessWindowEvents();
		}


		template <class ContextType>
		void    CreateContext(const ContextDescriptor& contextDesc)
		{
			static_assert(std::is_base_of<moe::GraphicsContext, ContextType>::value, "CreateContext only accepts classes derived from moe::GraphicsContext");
			static_assert(moe::TypeListIndexOf<typename WindowTraits<ConcreteWindow>::CompatibleContexts, ContextType>::value != -1,
				"This context type isn't supported by your Window type!");

			m_context = nullptr;
			window().template CreateConcreteContext<ContextType>(contextDesc);
		}

		void    DestroyWindow();


		void	FlushRemainingInputEvents()
		{
			m_handler.ModifyEventSink().Flush();
		}


		Handle  GetHandle()
		{
			return m_handle;
		}

	protected:
		~WindowBase() {}

		WindowAttributes                        m_attribs;
		std::unique_ptr<moe::GraphicsContext>   m_context;
		moe::IInputHandler						m_handler;

		Handle                                  m_handle;

	private:
		ConcreteWindow& window()
		{
			return static_cast<ConcreteWindow&>(*this);
		}
	};
}

#endif // MOE_WINDOW_H_