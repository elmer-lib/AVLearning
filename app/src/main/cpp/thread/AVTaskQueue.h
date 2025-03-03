//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVTASKQUEUE_H
#define AVLEARNING_AVTASKQUEUE_H

#include "base/BaseDefine.h"
#include "AVTaskQueueWithSingleThread.h"
#include "AVTaskQueueWithSingleGLThread.h"
#include <mutex>
#include <unordered_map>

enum TaskThreadID {
    NormalThread1,
    GLDecodeThread,
    GLDisplayThread,
};

enum TaskThreadType {
    Unknown,
    Normal,
    GLThread,
    GLThreadWithSurface,
};

struct TaskThreadInfo {
public:
    explicit TaskThreadInfo(TaskThreadType threadType, std::string threadName):
        mThreadType(threadType), mThreadName(threadName), mThread(nullptr) {}

    void createWorkThread() {
        std::lock_guard<std::mutex> lockGuard(mMutex);
        if (mThread == nullptr) {
            switch (mThreadType) {
                case TaskThreadType::Normal:
                    mThread = std::make_shared<AVTaskQueueWithSingleThread>(mThreadName);
                    break;
                case TaskThreadType::GLThread:
                    mThread = std::make_shared<AVTaskQueueWithSingleGLThread>(mThreadName, false);
                    break;
                case TaskThreadType::GLThreadWithSurface:
                    mThread = std::make_shared<AVTaskQueueWithSingleGLThread>(mThreadName, true);
                    break;
                default:
                    LOGE("createWorkThread error for mThreadType:%d", mThreadType);
            }
        }
    }

    void enqueue(AVTask&& task) { mThread->enqueue(std::forward<AVTask>(task)); }

    void surfaceCreated(ANativeWindow *window) { mThread->surfaceCreated(window); }

    void surfaceChanged(int width, int height) { mThread->surfaceChanged(width, height); }

    void surfaceDestroyed(void) { mThread->surfaceDestroyed(); }

    bool isValid() { return mThread != nullptr; }
private:
    TaskThreadType mThreadType = TaskThreadType::Normal;
    std::string mThreadName = "";
    std::shared_ptr<AVTaskQueueWithSingleThread> mThread = nullptr;
    std::mutex mMutex;
};

class AVTaskQueue {
public:
    static AVTaskQueue* getInstance();

    bool runSync(TaskThreadID threadId, AVTask task);

    bool runAsync(TaskThreadID threadId, AVTask task);

    void surfaceCreated(TaskThreadID threadId, ANativeWindow *window);

    void surfaceChanged(TaskThreadID threadId, int width, int height);

    void surfaceDestroyed(TaskThreadID threadId);

private:
    AVTaskQueue();

    ~AVTaskQueue();

private:
    static std::mutex instanceMutex;
    static AVTaskQueue* instance;

    std::mutex mTaskThreadMapMutex;
    std::unordered_map<TaskThreadID, std::shared_ptr<TaskThreadInfo>> mTaskThreadMap;

};


#endif //AVLEARNING_AVTASKQUEUE_H
