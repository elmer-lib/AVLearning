//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVEGLWINDOWSURFACE_H
#define AVLEARNING_AVEGLWINDOWSURFACE_H

#include "AVEGLSurfaceBase.h"

class AVEGLWindowSurface : public AVEGLSurfaceBase {

public:
    AVEGLWindowSurface(AVEGLCore *eglCore, ANativeWindow *window, bool releaseSurface);
    AVEGLWindowSurface(AVEGLCore *eglCore, ANativeWindow *window);
    // 释放资源
    void release();
    // 重新创建
    void recreate(AVEGLCore *eglCore);

private:
    ANativeWindow  *mSurface;
    bool mReleaseSurface;
};

#endif //AVLEARNING_AVEGLWINDOWSURFACE_H
