#include "ThreadPoolx.h"
#include <process.h>
#include <iostream>

std::vector<ThreadPoolx*> ThreadPoolx::threadPools;

ThreadPoolx* ThreadPoolx::Get(int index)
{
    return threadPools[index];
}

ThreadPoolx::ThreadPoolx(std::function<void(int)> threadFunc, const int numContexts)
    :numContexts(numContexts)
{
    threadPools.emplace_back(this);

    mThreadParameters = new ThreadParameter[numContexts];
    mThreadHandles = new HANDLE[numContexts];
    mWorkerStart = new HANDLE[numContexts];
    mWorkerEnd = new HANDLE[numContexts];

    for (size_t i = 0; i < numContexts; i++)
    {
        mThreadParameters[i].threadVecIndex = threadPools.size() - 1;
    }

    LoadContexts(threadFunc);
}

ThreadPoolx::~ThreadPoolx()
{
    delete mThreadParameters;
    delete mThreadHandles;
    delete mWorkerStart;
    delete mWorkerEnd;
}

void ThreadPoolx::StartWork()
{
    for (auto i = 0; i < numContexts; i++)
    {
        SetEvent(mWorkerStart[i]);
    }
}

void ThreadPoolx::WaitWork()
{
    WaitForMultipleObjects(numContexts, mWorkerEnd, true, INFINITE);
}

void ThreadPoolx::LoadContexts(std::function<void(int)> threadFunc)
{
    WorkerThread = threadFunc;

    struct threadwrapper
    {
        static unsigned int WINAPI thunk(LPVOID lpParameter)
        {
            ThreadParameter* parameter = reinterpret_cast<ThreadParameter*>(lpParameter);
            ThreadPoolx::Get(parameter->threadVecIndex)->Thread(parameter->threadIndex);
            return 0;
        }
    };

    for (auto i = 0; i < numContexts; i++)
    {
        mWorkerStart[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        mWorkerEnd[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        mThreadParameters[i].threadIndex = i;

        mThreadHandles[i] = reinterpret_cast<HANDLE>(_beginthreadex(
            nullptr,
            0,
            threadwrapper::thunk,
            reinterpret_cast<LPVOID>(&mThreadParameters[i]),
            0,
            nullptr));
    }
}

void ThreadPoolx::Thread(int threadIndex)
{
    while (threadIndex >= 0 && threadIndex < numContexts)
    {
        WaitForSingleObject(mWorkerStart[threadIndex], INFINITE);

        if (!WorkerThread) break;
        WorkerThread(threadIndex);

        SetEvent(mWorkerEnd[threadIndex]);
    }
    SetEvent(mWorkerEnd[threadIndex]);
}
