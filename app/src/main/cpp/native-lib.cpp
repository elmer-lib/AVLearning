#include <jni.h>
#include <string>
#include "codec/FFmpegDecoder.h"

extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_avlearning_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    FFmpegDecoder* decoder = new FFmpegDecoder();
    decoder->initDecoder("/data/data/com.example.avlearning/files/TG-2025-02-06-213757721.mp4", true, [](int64_t pts, std::shared_ptr<AVRGBAImage> frame){
        LOGI("decode video frame while pts is %" PRId64, pts);
        }, nullptr);
    int maxFrame = 10;
    while (maxFrame-- > 0) {
        decoder->decodeNextFrame();
    }


    return env->NewStringUTF(hello.c_str());
}