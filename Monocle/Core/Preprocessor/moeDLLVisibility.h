#ifndef MOE_DLL_VISIBILITY_H_
#define MOE_DLL_VISIBILITY_H_

// DLL Symbols visibility defines

#ifdef MOE_USE_DLL
    #ifdef MOE_WINDOWS
        #ifdef MOE_DLL_EXPORT
            #define MOE_DLL_API __declspec( dllexport )
        #else
            #define MOE_DLL_API __declspec( dllimport )
        #endif
    #elif defined(MOE_LINUX)
        #ifdef MOE_DLL_EXPORT
            #define MOE_DLL_API __attribute__ ((visibility ("default")))
        #else
            #define MOE_DLL_API
        #endif
    #endif // MOE_WINDOWS / MOE_LINUX
#else
    #define MOE_DLL_API
#endif

#endif // MOE_DLL_VISIBILITY_H_
