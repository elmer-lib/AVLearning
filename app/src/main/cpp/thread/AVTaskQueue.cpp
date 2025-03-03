//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVTaskQueue.h"

std::mutex AVTaskQueue::instanceMutex;
AVTaskQueue* AVTaskQueue::instance = nullptr;

AVTaskQueue::AVTaskQueue() {
    mTaskThreadMap.emplace(NormalThread1, std::make_shared<TaskThreadInfo>(TaskThreadType::Normal, "NormalThread1"));
    mTaskThreadMap.emplace(GLDecodeThread, std::make_shared<TaskThreadInfo>(TaskThreadType::GLThread, "GLDecodeThread"));
    mTaskThreadMap.emplace(GLDisplayThread, std::make_shared<TaskThreadInfo>(TaskThreadType::GLThreadWithSurface, "GLDisplayThread"));
}

AVTaskQueue::~AVTaskQueue() {
    std::lock_guard<std::mutex> lockGuard(instanceMutex);
    delete instance;
    instance = nullptr;
}

AVTaskQueue *AVTaskQueue::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lockGuard(instanceMutex);
        if (instance == nullptr) {
            volatile auto tmp = new AVTaskQueue;
            instance = tmp;
        }
    }
    return instance;
}

bool AVTaskQueue::runSync(TaskThreadID threadId, AVTask task) {
    bool isTaskFinish = false;
    std::mutex tmpMutex;
    std::condition_variable tmpCond;

    AVTask newTask = [&task, &isTaskFinish, &tmpMutex, &tmpCond]() {
        task();
        {
            std::lock_guard<std::mutex> lockGuard(tmpMutex);
            isTaskFinish = true;
        }
        tmpCond.notify_one();
    };

    auto ret = runAsync(threadId, newTask);
    if (!ret)
        return false;


    std::unique_lock<std::mutex> uniqueLock(tmpMutex);
    tmpCond.wait(uniqueLock, [&isTaskFinish] {
        return isTaskFinish;
    });
    return true;
}

bool AVTaskQueue::runAsync(TaskThreadID threadId, AVTask task) {
    std::lock_guard<std::mutex> lockGuard(mTaskThreadMapMutex);
    auto it = mTaskThreadMap.find(threadId);
    if (it == mTaskThreadMap.end()) {
        LOGE("runSync error because threadId:%d is unValid", threadId);
        return false;
    }
    if (!it->second->isValid()) {
        it->second->createWorkThread();
        if (!it->second->isValid()) {
            LOGE("createWorkThread error for threadId:%d", threadId);
            return false;
        }
    }
    it->second->enqueue(std::forward<AVTask>(task));
    return true;
}

void AVTaskQueue::surfaceCreated(TaskThreadID threadId, ANativeWindow *window) {
    std::lock_guard<std::mutex> lockGuard(mTaskThreadMapMutex);
    auto it = mTaskThreadMap.find(threadId);
    if (it == mTaskThreadMap.end()) {
        LOGE("surfaceCreated error because threadId:%d is unValid", threadId);
        return ;
    }
    if (!it->second->isValid()) {
        it->second->createWorkThread();
        if (!it->second->isValid()) {
            LOGE("surfaceCreated error for threadId:%d", threadId);
            return ;
        }
    }
    it->second->surfaceCreated(window);
}

void AVTaskQueue::surfaceChanged(TaskThreadID threadId, int width, int height) {
    std::lock_guard<std::mutex> lockGuard(mTaskThreadMapMutex);
    auto it = mTaskThreadMap.find(threadId);
    if (it == mTaskThreadMap.end()) {
        LOGE("surfaceCreated error because threadId:%d is unValid", threadId);
        return ;
    }
    if (!it->second->isValid()) {
        it->second->createWorkThread();
        if (!it->second->isValid()) {
            LOGE("surfaceCreated error for threadId:%d", threadId);
            return ;
        }
    }
    it->second->surfaceChanged(width, height);
}

void AVTaskQueue::surfaceDestroyed(TaskThreadID threadId) {
    std::lock_guard<std::mutex> lockGuard(mTaskThreadMapMutex);
    auto it = mTaskThreadMap.find(threadId);
    if (it == mTaskThreadMap.end()) {
        LOGE("surfaceCreated error because threadId:%d is unValid", threadId);
        return ;
    }
    if (!it->second->isValid()) {
        it->second->createWorkThread();
        if (!it->second->isValid()) {
            LOGE("surfaceCreated error for threadId:%d", threadId);
            return ;
        }
    }
    it->second->surfaceDestroyed();
}




