#include "Graphics/Context/Include/Windows/moeWGLContext.h"
#include "Core/Preprocessor/Include/moeAssert.h"
#include "Core/Log/Include/moeLogMacros.h"
#include "Core/Misc/Include/Windows/GetLastErrorAsString.h"

namespace moe
{
    WGLContext::WGLContext(HDC deviceContextHandle, CurrentFlag flag) :
        m_glContext(nullptr)
    {
        m_glContext = wglCreateContext(deviceContextHandle);
        if (!MOE_ENSURE(m_glContext != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "Unable to create WGL context! Last error: '%s'", moe::GetLastErrorAsString());
        }
        else
        {
            SetCurrentState(deviceContextHandle, flag);
        }

    }

    WGLContext::WGLContext(HDC deviceContextHandle, const int* wglAttributes, CurrentFlag flag)
    {
        m_glContext = wglCreateContextAttribsARB(deviceContextHandle, NULL, wglAttributes);
        if (!MOE_ENSURE(m_glContext != nullptr))
        {
            MOE_ERROR(moe::ChanGraphics, "Unable to create extensible WGL context! Last error: '%s'", moe::GetLastErrorAsString());
        }
        else
        {
            SetCurrentState(deviceContextHandle, flag);
        }
    }

    void WGLContext::SetCurrentState(HDC deviceContextHandle, CurrentFlag flag)
    {
        if ((flag & Current) == Current)
        {
            m_isCurrent = true;
            wglMakeCurrent(deviceContextHandle, m_glContext);
        }
        else
        {
            m_isCurrent = false;
            wglMakeCurrent(nullptr, nullptr);
        }
    }

    WGLContext::~WGLContext()
    {
        if (m_isCurrent)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_glContext);
        }
    }

}