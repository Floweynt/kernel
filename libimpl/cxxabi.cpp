#include <klog/klog.h>
extern "C"
{
#define ATEXIT_FUNC_MAX 128

    typedef unsigned uarch_t;

    struct atexit_fn_t
    {
        void (*destructorFunc)(void*);
        void* objPtr;
        void* dsoHandle;
    };

    void __cxa_pure_virtual() { klog::panic("invoked pure virtual function"); }

    atexit_fn_t __atexitFuncs[ATEXIT_FUNC_MAX];
    uarch_t __atexitFuncCount = 0;

    void* __dso_handle = 0;

    int __cxa_atexit(void (*f)(void*), void* objptr, void* dso)
    {
        if (__atexitFuncCount >= ATEXIT_FUNC_MAX)
            return -1;
        __atexitFuncs[__atexitFuncCount].destructorFunc = f;
        __atexitFuncs[__atexitFuncCount].objPtr = objptr;
        __atexitFuncs[__atexitFuncCount].dsoHandle = dso;
        __atexitFuncCount++;
        return 0;
    }

    void __cxa_finalize(void* f)
    {
        signed i = __atexitFuncCount;
        if (!f)
        {
            while (i--)
            {
                if (__atexitFuncs[i].destructorFunc)
                    (*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
            }
            return;
        }

        for (; i >= 0; i--)
        {
            if (__atexitFuncs[i].destructorFunc == f)
            {
                (*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
                __atexitFuncs[i].destructorFunc = 0;
            }
        }
    }
}
