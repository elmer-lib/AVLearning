//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVEGLWindowSurface.h"
#include <assert.h>

AVEGLWindowSurface::AVEGLWindowSurface(AVEGLCore *eglCore, ANativeWindow *window, bool releaseSurface)
        : AVEGLSurfaceBase(eglCore) {
    mSurface = window;
    createWindowSurface(mSurface);
    mReleaseSurface = releaseSurface;
}

AVEGLWindowSurface::AVEGLWindowSurface(AVEGLCore *eglCore, ANativeWindow *window)
        : AVEGLSurfaceBase(eglCore) {
    createWindowSurface(window);
    mSurface = window;
}

void AVEGLWindowSurface::release() {
    releaseEglSurface();
    if (mSurface != NULL) {
        ANativeWindow_release(mSurface);
        mSurface = NULL;
    }
}

void AVEGLWindowSurface::recreate(AVEGLCore *eglCore) {
    assert(mSurface != NULL);
    if (mSurface == NULL) {
        LOGE("not yet implemented ANativeWindow");
        return;
    }
    mEglCore = eglCore;
    createWindowSurface(mSurface);
}
