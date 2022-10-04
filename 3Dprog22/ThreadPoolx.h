#pragma once
#include <Windows.h>
#include <functional>

class ThreadPoolx
{
	struct ThreadParameter
	{
		int threadIndex;
		int threadVecIndex;
	};

	ThreadParameter* mThreadParameters;

	HANDLE* mThreadHandles;

	HANDLE* mWorkerStart;
	HANDLE* mWorkerEnd;

	std::function<void(int)> WorkerThread{ nullptr };

	static ThreadPoolx* Get(int index);

	void LoadContexts(std::function<void(int)> threadFunc);

	void Thread(int index);

	const int numContexts;
public:

	static std::vector<ThreadPoolx*> threadPools;

	ThreadPoolx(std::function<void(int)> threadFunc, const int numContexts);
	~ThreadPoolx();
	
	void StartWork();
	void WaitWork();
};