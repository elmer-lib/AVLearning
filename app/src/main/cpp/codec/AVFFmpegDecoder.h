//
// Created by 周豪豪 on 2025/3/2.
//

#ifndef AVLEARNING_AVFFMPEGDECODER_H
#define AVLEARNING_AVFFMPEGDECODER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavresample/avresample.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

#include "base/BaseDefine.h"
#include "AVImage.h"

using AV_VIDEO_DECODE_CALLBACL = std::function<void(int64_t , std::shared_ptr<AVRGBAImage>)>;
using AV_AUDIO_DECODE_CALLBACL = std::function<void(int64_t, std::shared_ptr<AVRGBAImage>)>;

class AVFFmpegDecoder {
public:
    AVResult initDecoder(const std::string& filePath, bool isVideo,
                         AV_VIDEO_DECODE_CALLBACL videoDecodeCallback,
                         AV_AUDIO_DECODE_CALLBACL audioDecodeCallback);

    AVResult unInitDecoder();

    AVResult seekToTime(int64_t time);

    AVResult decodeNextFrame();

private:
    AVResult parseAVInfo();

    AVResult preProcessVideoFrame();

    AVResult preProcessAudioFrame();

private:
    bool mIsDecodeValid = false;

    bool mIsVideoMode = true;
    std::string mFilePath;
    AV_VIDEO_DECODE_CALLBACL mVideoDecodeCallback = nullptr;
    std::shared_ptr<AVRGBAImage> mAVRGBAImage = nullptr;
    AV_AUDIO_DECODE_CALLBACL mAudioDecodeCallback = nullptr;

    // ffmpeg context info
    int avStreamIndex = -1;
    AVFormatContext* avFormatContext = nullptr;
    AVCodecContext* avCodecContext = nullptr;
    AVPacket* avPacket = nullptr;
    AVFrame* avFrame = nullptr;
    SwsContext* swsContext = nullptr;

    // video info
    int64_t mVideoWidth = 0;
    int64_t mVideoHeight = 0;
    AVPixelFormat mVideoPixelFormat = AVPixelFormat::AV_PIX_FMT_NONE;

    // audio info

};


#endif //AVLEARNING_AVFFMPEGDECODER_H
