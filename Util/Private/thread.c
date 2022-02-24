#include "thread.h"

#if defined(_WIN32)

    struct _win32_data
    {
        void *(*function)(void *);
        void * arg;
    };

    typedef struct _win32_data win32_data_t;

    static DWORD WINAPI _win32_thread_start(void * data)
    {
        win32_data_t * win32_data = data;
        void *(*function)(void *) = win32_data->function;
        void * arg = win32_data->arg;

        free(data);

        function(arg);
        return 0;
    }

#endif

bool thread_create(thread_t * thread, void *(*function)(void *), void * arg)
{
    if (!thread || !function) {
        return false;
    }

    #if defined(_WIN32)

        win32_data_t * win32_data = malloc(sizeof(win32_data_t));
        win32_data->function = function;
        win32_data->arg = arg;

        *thread = CreateThread(
            NULL,
            0,
            _win32_thread_start,
            win32_data,
            0,
            NULL
        );

        if (!*thread) {
            return false;
        }

    #else

        if (pthread_create(thread, NULL, function, arg) != 0) {
            return false;
        }

    #endif

    return true;
}

void thread_join(thread_t thread)
{
    #if defined(_WIN32)

        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);

    #else

        pthread_join(thread, NULL);

    #endif
}

bool thread_mutex_init(thread_mutex_t * mutex)
{
    if (!mutex) {
        return false;
    }

    #if defined(_WIN32)

        InitializeCriticalSection(mutex);

    #else

        if (pthread_mutex_init(mutex) != 0) {
            return false;
        }

    #endif

    return true;
}

void thread_mutex_term(thread_mutex_t * mutex)
{
    #if defined(_WIN32)

        DeleteCriticalSection(mutex);

    #else

        pthread_mutex_destroy(mutex);

    #endif
}

bool thread_mutex_lock(thread_mutex_t * mutex)
{
    if (!mutex) {
        return false;
    }

    #if defined(_WIN32)

        EnterCriticalSection(mutex);

    #else

        pthread_mutex_lock(mutex);

    #endif

    return true;
}

bool thread_mutex_unlock(thread_mutex_t * mutex)
{
    if (!mutex) {
        return false;
    }

    #if defined(_WIN32)

        LeaveCriticalSection(mutex);

    #else

        pthread_mutex_unlock(mutex);

    #endif

    return true;
}