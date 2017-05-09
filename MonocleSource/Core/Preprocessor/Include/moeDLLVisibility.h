#ifndef MOE_DLL_VISIBILITY_H_
#define MOE_DLL_VISIBILITY_H_

// DLL Symbols visibility defines
// TODO: Linux compatibility check
#ifdef MOE_USE_DLL
    #ifdef MOE_DLL_EXPORT
        #define MOE_DLL_API __declspec( dllexport )
    #else
        #define MOE_DLL_API __declspec( dllimport )
    #endif
#else
    #define MOE_DLL_API
#endif

#endif // MOE_DLL_VISIBILITY_H_
