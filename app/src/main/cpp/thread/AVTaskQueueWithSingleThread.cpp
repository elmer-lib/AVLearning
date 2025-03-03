//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVTaskQueueWithSingleThread.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
void SetThreadName(const char* threadName) {
    SetThreadDescription(GetCurrentThread(), threadName);
}
#elif defined(__linux__)
#include <pthread.h>
void SetThreadName(const char* threadName) {
    pthread_setname_np(pthread_self(), threadName);
}
#else
void SetThreadName(const char*) {
    // 不支持线程命名
}
#endif

AVTaskQueueWithSingleThread::AVTaskQueueWithSingleThread(const std::string &threadName) {
    mWorker = std::make_shared<std::thread>([this, threadName](){
        SetThreadName(threadName.c_str());
        while (true) {
            AVTask task;
            {
                std::unique_lock<std::mutex> uniqueLock(this->mQueueMutex);
                this->mQueueCond.wait(uniqueLock, [this](){
                    return this->mIsStop || !this->mTaskQueue.empty();
                });

                if (this->mIsStop && this->mTaskQueue.empty())
                    return ;
                task = std::move(this->mTaskQueue.front());
                this->mTaskQueue.pop();
                if (this->mTaskQueue.empty()) {
                    mQueueCondForRelease.notify_all();
                }
            }
            task();
        }
    });
}

AVTaskQueueWithSingleThread::~AVTaskQueueWithSingleThread() {
    {
        std::unique_lock<std::mutex> uniqueLock(mQueueMutex);
        mQueueCondForRelease.wait(uniqueLock, [this](){
            return mTaskQueue.empty();
        });
        mIsStop = true;
    }
    mQueueCond.notify_all();
    if (mWorker->joinable())
        mWorker->join();
}

bool AVTaskQueueWithSingleThread::enqueue(AVTask &&task) {
    {
        std::unique_lock<std::mutex> uniqueLock(mQueueMutex);
        if (mIsStop)
            return false;
        mTaskQueue.emplace(std::move(task));
    }
    mQueueCond.notify_one();
    return true;
}
