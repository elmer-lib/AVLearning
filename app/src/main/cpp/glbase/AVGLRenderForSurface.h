//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVGLRENDERFORSURFACE_H
#define AVLEARNING_AVGLRENDERFORSURFACE_H


#include <android/native_window.h>
#include "glbase/AVTriangle.h"
#include "egl/AVEGLWindowSurface.h"

class AVGLRenderForSurface {
public:
    AVGLRenderForSurface() = default;

    virtual ~AVGLRenderForSurface();

    void surfaceCreated(ANativeWindow *window);

    void surfaceChanged(int width, int height);

    void surfaceDestroyed(void);

private:
    AVEGLCore *mEglCore = nullptr;
    AVEGLWindowSurface *mWindowSurface = nullptr;
    AVTriangle *mTriangle = nullptr;
};

#endif //AVLEARNING_AVGLRENDERFORSURFACE_H
