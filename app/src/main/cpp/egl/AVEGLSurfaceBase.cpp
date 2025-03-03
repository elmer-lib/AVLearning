//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVEGLSurfaceBase.h"

#include <assert.h>
#include <GLES2/gl2.h>

AVEGLSurfaceBase::AVEGLSurfaceBase(AVEGLCore *eglCore) : mEglCore(eglCore) {
    mEglSurface = EGL_NO_SURFACE;
}

/**
 * 创建显示的Surface
 * @param nativeWindow
 */
void AVEGLSurfaceBase::createWindowSurface(ANativeWindow *nativeWindow) {
    assert(mEglSurface == EGL_NO_SURFACE);
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGE("surface already created");
        return;
    }
    mEglSurface = mEglCore->createWindowSurface(nativeWindow);
}

/**
 * 创建离屏surface
 * @param width
 * @param height
 */
void AVEGLSurfaceBase::createOffscreenSurface(int width, int height) {
    assert(mEglSurface == EGL_NO_SURFACE);
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGE("surface already created");
        return;
    }
    mEglSurface = mEglCore->createOffscreenSurface(width, height);
    mWidth = width;
    mHeight = height;
}

/**
 * 获取宽度
 * @return
 */
int AVEGLSurfaceBase::getWidth() {
    if (mWidth < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_WIDTH);
    } else {
        return mWidth;
    }
}

/**
 * 获取高度
 * @return
 */
int AVEGLSurfaceBase::getHeight() {
    if (mHeight < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_HEIGHT);
    } else {
        return mHeight;
    }
}

/**
 * 释放EGLSurface
 */
void AVEGLSurfaceBase::releaseEglSurface() {
    mEglCore->releaseSurface(mEglSurface);
    mEglSurface = EGL_NO_SURFACE;
    mWidth = mHeight = -1;
}

/**
 * 切换到当前EGLContext
 */
void AVEGLSurfaceBase::makeCurrent() {
    mEglCore->makeCurrent(mEglSurface);
}

/**
 * 交换到前台显示
 * @return
 */
bool AVEGLSurfaceBase::swapBuffers() {
    bool result = mEglCore->swapBuffers(mEglSurface);
    if (!result) {
        LOGD("WARNING: swapBuffers() failed");
    }
    return result;
}

/**
 * 设置当前时间戳
 * @param nsecs
 */
void AVEGLSurfaceBase::setPresentationTime(long nsecs) {
    mEglCore->setPresentationTime(mEglSurface, nsecs);
}

/**
 * 获取当前像素
 * @return
 */
char* AVEGLSurfaceBase::getCurrentFrame() {
    char *pixels = NULL;
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return pixels;
}
