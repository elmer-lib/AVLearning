//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVGLRENDERFORNORMAL_H
#define AVLEARNING_AVGLRENDERFORNORMAL_H

#include "egl/AVEGLOffscreenSurface.h"

class AVGLRenderForNormal {
public:
    AVGLRenderForNormal() = default;

    virtual ~AVGLRenderForNormal();

    void init(int width, int height);

    void unInit();

private:
    AVEGLCore *mEglCore = nullptr;
    AVEGLOffscreenSurface *mOffscreenSurface = nullptr;
};


#endif //AVLEARNING_AVGLRENDERFORNORMAL_H
