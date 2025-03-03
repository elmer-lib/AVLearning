//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVTASKQUEUEWITHSINGLETHREAD_H
#define AVLEARNING_AVTASKQUEUEWITHSINGLETHREAD_H

#include <thread>
#include <queue>
#include <future>
#include <condition_variable>
#include "base/BaseDefine.h"
#include <optional>
#include "glbase/AVGLRenderForSurface.h"

#include <iostream>
#include <thread>
#include <chrono>

using AVTask = std::function<void()>;

class AVTaskQueueWithSingleThread {
public:
    AVTaskQueueWithSingleThread(const std::string& threadName);

    virtual ~AVTaskQueueWithSingleThread();

    bool enqueue(AVTask&& task);

    virtual void surfaceCreated(ANativeWindow *window) {}

    virtual void surfaceChanged(int width, int height) {}

    virtual void surfaceDestroyed(void) {}

private:
    std::shared_ptr<std::thread> mWorker = nullptr;
    std::queue<AVTask> mTaskQueue;

    std::mutex mQueueMutex;
    std::condition_variable mQueueCond;
    std::condition_variable mQueueCondForRelease;

    bool mIsStop;
};


#endif //AVLEARNING_AVTASKQUEUEWITHSINGLETHREAD_H
