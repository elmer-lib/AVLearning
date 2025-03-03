//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVGLRenderForSurface.h"

AVGLRenderForSurface::~AVGLRenderForSurface() {
    if (mEglCore) {
        mEglCore->release();
        delete mEglCore;
        mEglCore = NULL;
    }
}

void AVGLRenderForSurface::surfaceCreated(ANativeWindow *window) {
    if (mEglCore == NULL) {
        mEglCore = new AVEGLCore(NULL, FLAG_RECORDABLE);
    }
    mWindowSurface = new AVEGLWindowSurface(mEglCore, window, false);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    mWindowSurface->makeCurrent();
    mTriangle = new AVTriangle();
    mTriangle->init();
}

void AVGLRenderForSurface::surfaceChanged(int width, int height) {
    mWindowSurface->makeCurrent();
    mTriangle->onDraw(width, height);
    mWindowSurface->swapBuffers();
}

void AVGLRenderForSurface::surfaceDestroyed() {
    if (mTriangle) {
        mTriangle->destroy();
        delete mTriangle;
        mTriangle = NULL;
    }
    if (mWindowSurface) {
        mWindowSurface->release();
        delete mWindowSurface;
        mWindowSurface = NULL;
    }
    if (mEglCore) {
        mEglCore->release();
        delete mEglCore;
        mEglCore = NULL;
    }
}
