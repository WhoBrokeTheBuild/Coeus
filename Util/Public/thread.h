#ifndef COEUS_UTIL_THREAD_H
#define COEUS_UTIL_THREAD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)

    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    typedef HANDLE thread_t;

    typedef CRITICAL_SECTION thread_mutex_t;

#else

    #include <pthread.h>

    typedef pthread_t thread_t;

    typedef pthread_mutex_t thread_mutex_t;

#endif

bool thread_create(thread_t * thread, void *(*function)(void *), void * arg);

void thread_join(thread_t thread);

bool thread_detach(thread_t thread);

bool thread_mutex_init(thread_mutex_t * mutex);

void thread_mutex_term(thread_mutex_t * mutex);

bool thread_mutex_lock(thread_mutex_t * mutex);

bool thread_mutex_unlock(thread_mutex_t * mutex);

#endif // COEUS_UTIL_THREAD_H