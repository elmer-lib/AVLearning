//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVJPEGENCODE_H
#define AVLEARNING_AVJPEGENCODE_H
/*
 * 主要功能：libjpeg-turbo包含两种API，一种是libjpeg-turbo，另一种是libjpeg。
 *      当两种API都用于执行类似的操作时，这两种API没有显著的性能优势。具体的api使用方法可以参考 doc/index.html 网页的Modules下面的TurboJPEG链接的内容。
 *      libjpeg-turbo 使用起来更简单。
 *      libjpeg API 的存在是为了与API/ABI兼容，又与libjpeg v6b在数学上兼容。它还可以可选地配置为与libjpeg v7和v8兼容的API/ABI。
 * libjpeg-turbo库提供的功能主要分为4类，包括编码，解码，格式转换，和读写bmp文件。
 *      编码：将一个rgb图像编码为jpeg文件、将一个yuv图像编码为jpeg文件
 *      解码：获取jpeg文件宽高，采样率，色彩空间等信息、将一个jpeg文件解码为rgb图像、将一个jpeg文件解码为yuv图像
 *      格式转换：rgb图像转为yuv图像、yuv图像转rgb图像
 *      bmp：读取一个bmp文件到内存、将图像数据保存为bmp文件
 */

#include "stdio.h"
#include "jpeglib.h"
#include "setjmp.h"
#include "base/BaseDefine.h"

class AVJpegEncode {
public:
    static AVResult writeJpegFile(const char* filePath, int quality, int imageWidth, int imageHeight, uint8_t* imageData);

    static AVResult readJpegFile(const char* filePath, int& outImageWidth, int outImageHeight, uint8_t* outImageData);
};


#endif //AVLEARNING_AVJPEGENCODE_H
