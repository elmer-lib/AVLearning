//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVEGLOffscreenSurface.h"

AVEGLOffscreenSurface::AVEGLOffscreenSurface(AVEGLCore *eglCore, int width, int height)
        : AVEGLSurfaceBase(eglCore) {
    createOffscreenSurface(width, height);
}

void AVEGLOffscreenSurface::release() {
    releaseEglSurface();
}
