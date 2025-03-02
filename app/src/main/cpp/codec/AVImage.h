//
// Created by 周豪豪 on 2025/3/2.
//

#ifndef AVLEARNING_AVIMAGE_H
#define AVLEARNING_AVIMAGE_H

#include "base/BaseDefine.h"

class AVRGBAImage {
public:
    AVRGBAImage(int width, int height) {
        if (width <= 0 || height <= 0 || mRGBABuffer != nullptr)
            return ;

        const size_t page_size = 4096; // 一般页面大小为 4096 字节
        mRGBABuffer = (uint8_t*)aligned_alloc(page_size, width * height * 4);
        if (mRGBABuffer == nullptr) {
            LOGE("aligned_alloc error for %d byte", width * height * 4);
            return ;
        }
        mWidth = width;
        mHeight = height;
    }

    ~AVRGBAImage() {
        if (mRGBABuffer != nullptr) {
            free(mRGBABuffer);
            mRGBABuffer = nullptr;
        }
    }


    uint8_t* getRawPtr() { return mRGBABuffer; }

    int getWidth() { return mWidth; }

    int getHeight() { return mHeight; }

private:
    int mWidth = -1;
    int mHeight = -1;
    uint8_t* mRGBABuffer = nullptr;
};


#endif //AVLEARNING_AVIMAGE_H
