#ifndef MOE_AUTO_GLAD_GLX_LOADER_H_
#define MOE_AUTO_GLAD_GLX_LOADER_H_

// An automatic, RAII GLX loader using Glad.
// This is a bit of a hack because of glad default behavior.
// The thing is that on Linux, glad will dlopen() the GL shared object to initialize function pointers, then close it.
// But by closing it, we also free the library resources. Something must keep a handle on the lib to prevent that.
// So this RAII auto loader just keeps a dummy handle to the GL lib, and closes it on destruct.
// It should ALWAYS be destroyed AFTER the last call to gl/glX* functions !! Otherwise prepare to crash.

namespace moe
{
    class AutoGladGLXLoader
    {
    public:
        AutoGladGLXLoader();
        ~AutoGladGLXLoader();

        bool    IsLoaded() const;

    private:
        void*   m_libGLHandle;
    };

}


#endif // MOE_AUTO_GLAD_GLX_LOADER_H_
