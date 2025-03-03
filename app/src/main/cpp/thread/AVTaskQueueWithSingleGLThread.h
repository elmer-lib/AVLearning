//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVTASKQUEUEWITHSINGLEGLTHREAD_H
#define AVLEARNING_AVTASKQUEUEWITHSINGLEGLTHREAD_H

#include "AVTaskQueueWithSingleThread.h"
#include "glbase/AVGLRenderForNormal.h"
#include "glbase/AVGLRenderForSurface.h"

class AVTaskQueueWithSingleGLThread: public AVTaskQueueWithSingleThread {
public:
    AVTaskQueueWithSingleGLThread(const std::string& threadName, bool isWithSurface);

    void surfaceCreated(ANativeWindow *window) override;

    void surfaceChanged(int width, int height) override;

    void surfaceDestroyed(void) override;

private:
    bool mIsWithSurface = false;
    std::shared_ptr<AVGLRenderForNormal> mAVGLRenderForNormal = nullptr;
    std::shared_ptr<AVGLRenderForSurface> mAVGLRenderForSurface = nullptr;
};


#endif //AVLEARNING_AVTASKQUEUEWITHSINGLEGLTHREAD_H
