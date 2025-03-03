//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVEGLSURFACEBASE_H
#define AVLEARNING_AVEGLSURFACEBASE_H

#include "EGL/AVEGLCore.h"

class AVEGLSurfaceBase {

public:
    AVEGLSurfaceBase(AVEGLCore *eglCore);
    // 创建窗口Surface
    void createWindowSurface(ANativeWindow *nativeWindow);
    // 创建离屏Surface
    void createOffscreenSurface(int width, int height);
    // 获取宽度
    int getWidth();
    // 获取高度
    int getHeight();
    // 释放EGLSurface
    void releaseEglSurface();
    // 切换到当前上下文
    void makeCurrent();
    // 交换缓冲区，显示图像
    bool swapBuffers();
    // 设置显示时间戳
    void setPresentationTime(long nsecs);
    // 获取当前帧缓冲
    char *getCurrentFrame();

protected:
    AVEGLCore *mEglCore;
    EGLSurface mEglSurface;
    int mWidth;
    int mHeight;

};

#endif //AVLEARNING_AVEGLSURFACEBASE_H
