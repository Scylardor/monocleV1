#include "Graphics/OpenGL/Linux/moeAutoGladGLXLoader.h"

#include <dlfcn.h>

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLogMacros.h"

namespace moe
{
    AutoGladGLXLoader::AutoGladGLXLoader() :
        m_libGLHandle(nullptr)
    {
        MOE_INFO(moe::ChanDebug, "Automatic GLX loader loading lib GL.");
        m_libGLHandle = dlopen("libGL.so", RTLD_LAZY);
        MOE_DEBUG_ASSERT(IsLoaded());
    }

    AutoGladGLXLoader::~AutoGladGLXLoader()
    {
        if (IsLoaded())
        {
            MOE_INFO(moe::ChanDebug, "Automatic GLX loader unloading lib GL.");
            MOE_ASSERT(dlclose(m_libGLHandle) == 0);
        }
    }

    bool    AutoGladGLXLoader::IsLoaded() const
    {
        return (m_libGLHandle != nullptr);
    }
}