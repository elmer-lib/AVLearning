//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVGLRenderForNormal.h"

AVGLRenderForNormal::~AVGLRenderForNormal() {
    unInit();
}

void AVGLRenderForNormal::init(int width, int height) {
    if (mEglCore == NULL) {
        mEglCore = new AVEGLCore(NULL, FLAG_RECORDABLE);
    }
    mOffscreenSurface = new AVEGLOffscreenSurface(mEglCore, width, height);
    assert(mOffscreenSurface != NULL && mEglCore != NULL);
    mOffscreenSurface->makeCurrent();
}

void AVGLRenderForNormal::unInit() {
    if (mOffscreenSurface) {
        mOffscreenSurface->release();
        delete mOffscreenSurface;
        mOffscreenSurface = NULL;
    }
    if (mEglCore) {
        mEglCore->release();
        delete mEglCore;
        mEglCore = NULL;
    }
}
