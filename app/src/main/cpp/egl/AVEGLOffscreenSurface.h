//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVEGLOFFSCREENSURFACE_H
#define AVLEARNING_AVEGLOFFSCREENSURFACE_H

#include "AVEGLSurfaceBase.h"

class AVEGLOffscreenSurface : public AVEGLSurfaceBase {
public:
    AVEGLOffscreenSurface(AVEGLCore *eglCore, int width, int height);
    void release();
};


#endif //AVLEARNING_AVEGLOFFSCREENSURFACE_H
