//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVTaskQueueWithSingleGLThread.h"

AVTaskQueueWithSingleGLThread::AVTaskQueueWithSingleGLThread(const std::string &threadName,
                                                             bool isWithSurface):
        mIsWithSurface(isWithSurface),
        AVTaskQueueWithSingleThread(threadName)
{
    if (!mIsWithSurface) {
        enqueue([this]() {
            mAVGLRenderForNormal = std::make_shared<AVGLRenderForNormal>();
            mAVGLRenderForNormal->init(64, 64);
        });
    } else {
        enqueue([this]() {
            mAVGLRenderForSurface = std::make_shared<AVGLRenderForSurface>();
        });
    }
}

void AVTaskQueueWithSingleGLThread::surfaceCreated(ANativeWindow *window) {
    if (!mIsWithSurface) {
        LOGE("surfaceCreated error because mIsWithSurface is false");
        return;
    }
    enqueue([this, window]() {
        mAVGLRenderForSurface->surfaceCreated(window);
    });
}

void AVTaskQueueWithSingleGLThread::surfaceChanged(int width, int height) {
    if (!mIsWithSurface) {
        LOGE("surfaceChanged error because mIsWithSurface is false");
        return;
    }
    enqueue([this, width, height]() {
        mAVGLRenderForSurface->surfaceChanged(width, height);
    });
}

void AVTaskQueueWithSingleGLThread::surfaceDestroyed(void) {
    if (!mIsWithSurface) {
        LOGE("surfaceDestroyed error because mIsWithSurface is false");
        return;
    }
    enqueue([this]() {
        mAVGLRenderForSurface->surfaceDestroyed();
    });
}
