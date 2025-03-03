#include <jni.h>
#include <string>
#include "codec/AVFFmpegDecoder.h"
#include "jpeg/AVJpegEncode.h"
#include "thread/AVTaskQueue.h"

extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_avlearning_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    AVFFmpegDecoder* decoder = new AVFFmpegDecoder();
    decoder->initDecoder("/data/data/com.example.avlearning/files/TG-2025-02-06-213757721.mp4", true, [](int64_t pts, std::shared_ptr<AVRGBAImage> frame){
        LOGI("decode video frame while pts is %" PRId64, pts);
        AVJpegEncode::writeJpegFile(("/data/data/com.example.avlearning/files/" + std::to_string(pts) + ".jpeg").c_str(), 100, frame->getWidth(), frame->getHeight(), frame->getRawPtr());
        }, nullptr);
    int maxFrame = 10;
    while (maxFrame-- > 0) {
        decoder->decodeNextFrame();
    }

    /*
     * 预期打印 02 - 01 - 03 - 04
     *
     * */
    AVTaskQueue::getInstance()->runAsync(TaskThreadID::NormalThread1, [](){
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        LOGI("taskQueue test 01");
    });
    LOGI("taskQueue test 02");
    AVTaskQueue::getInstance()->runSync(TaskThreadID::NormalThread1, [](){
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        LOGI("taskQueue test 03");
    });
    LOGI("taskQueue test 04");

    return env->NewStringUTF(hello.c_str());
}