//
// Created by 周豪豪 on 2025/3/2.
//

#include "FFmpegDecoder.h"

/*
 * 整个初始化分为三大步
 * 1. 初始化formatContext，并根据封装信息找到所需要的流avStream
 * 2. 根据avSteam找到相对应的codec并初始化codecContext
 * 3. 打开codec
 * */

AVResult FFmpegDecoder::initDecoder(const std::string& filePath, bool isVideo,
                                    AV_VIDEO_DECODE_CALLBACL videoDecodeCallback,
                                    AV_AUDIO_DECODE_CALLBACL audioDecodeCallback) {
    mFilePath = filePath;
    mIsVideoMode = isVideo;
    if (mIsVideoMode) {
        mVideoDecodeCallback = videoDecodeCallback;
    } else {
        mAudioDecodeCallback = audioDecodeCallback;
    }

    int ret = 0;
    // 1.1 open input file, and allocate format context
    ret = avformat_open_input(&avFormatContext, filePath.c_str(), NULL, NULL);
    if (ret < 0) {
        LOGE("avformat_open_input error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    // 1.2 retrieve stream information
    ret = avformat_find_stream_info(avFormatContext, NULL);
    if (ret < 0) {
        LOGE("avformat_find_stream_info error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    // 1.3 find video or audio stream
    ret = av_find_best_stream(avFormatContext,
                              mIsVideoMode ? AVMediaType::AVMEDIA_TYPE_VIDEO : AVMediaType::AVMEDIA_TYPE_AUDIO,
                              -1, -1, NULL, 0);
    if (ret < 0) {
        LOGE("av_find_best_stream error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    avStreamIndex = ret;
    AVStream* avStream = avFormatContext->streams[avStreamIndex];
    // 2.1 find best codec for the special stream
    AVCodec* avCodec = avcodec_find_decoder(avStream->codecpar->codec_id);
    if (avCodec == nullptr) {
        LOGE("avcodec_find_decoder error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    // 2.2 Allocate a codec context for the decoder
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (avCodecContext == nullptr) {
        LOGE("avcodec_alloc_context3 error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }
    // 2.3 init codec context for the decoder
    ret = avcodec_parameters_to_context(avCodecContext, avStream->codecpar);
    if (ret < 0) {
        LOGE("avcodec_parameters_to_context error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    // 3. open the decoder
    ret = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (ret < 0) {
        LOGE("avcodec_open2 error while filePath is %s and ret is %d", filePath.c_str(), ret);
        return ret;
    }

    // dump info
    /* ic：指向AVFormatContext的指针，这个结构包含了要打印的媒体格式的信息。
     * index：通常设置为0，用于标识打印信息时的索引号，主要用于区分当处理多个文件或流时的输出。
     * url：媒体流的URL或文件名。
     * is_output: 一个标志，指示传递的AVFormatContext是用于输出（即编码和写入文件）还是输入（即解码和读取文件）。当is_output为0时，表示这是一个输入格式上下文，通常是已经打开用于读取的文件。当is_output为非0时，表示这是一个输出格式上下文，通常是准备用于写入的文件。
     * */
    av_dump_format(avFormatContext, 0, filePath.c_str(), 0);

    if (avPacket == nullptr)
        avPacket = av_packet_alloc();
    if (avFrame == nullptr)
        avFrame = av_frame_alloc();
    if (avPacket == nullptr || avFrame == nullptr) {
        LOGE("av_packet_alloc or av_frame_alloc error while filePath is %s", filePath.c_str());
        return AV_ERROR;
    }

    ret = parseAVInfo();
    if (ret != AV_OK) {
        LOGE("parseAVInfo error while filePath is %s", filePath.c_str());
        return AV_ERROR;
    }

    mIsDecodeValid = true;
    return AV_OK;
}

AVResult FFmpegDecoder::unInitDecoder() {
    if (avCodecContext != nullptr) {
        avcodec_free_context(&avCodecContext);
        avCodecContext = nullptr;
    }

    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
        avFormatContext = nullptr;
    }

    if (avPacket != nullptr) {
        av_packet_free(&avPacket);
        avPacket = nullptr;
    }

    if (avFrame != nullptr) {
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }

    if (swsContext != nullptr) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    return 0;
}

AVResult FFmpegDecoder::seekToTime(int64_t time) {
    if (!mIsDecodeValid) {
        LOGE("seekToTime error because mIsDecodeValid is false");
        return AV_ERROR;
    }
    return 0;
}

AVResult FFmpegDecoder::decodeNextFrame() {
    if (!mIsDecodeValid) {
        LOGE("seekToTime error because mIsDecodeValid is false");
        return AV_ERROR;
    }

    int ret = AV_OK;
    // read frames from the file
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index != avStreamIndex)
            continue;

        // submit the packet to the decoder
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret == AVERROR_EOF) {
            LOGI("EOF for filePath:%s", mFilePath.c_str());
            av_packet_unref(avPacket);
            return AV_OK;
        } else if (ret == AVERROR(EAGAIN)) {
            LOGI("input is not accepted in the current state - user must read output with avcodec_receive_frame() for filePath:%s", mFilePath.c_str());
        } else if (ret < 0) {
            LOGE("avcodec_send_packet error while filePath is %s and ret is %d", mFilePath.c_str(), ret);
            av_packet_unref(avPacket);
            return ret;
        }

        // get all the available frames from the decoder
        while (true) {
            ret = avcodec_receive_frame(avCodecContext, avFrame);
            if (ret == AVERROR_EOF) {
                LOGI("EOF for filePath:%s", mFilePath.c_str());
                av_frame_unref(avFrame);
                av_packet_unref(avPacket);
                return AV_OK;
            } else if (ret == AVERROR(EAGAIN)) {
                LOGI("EOF for filePath:%s", mFilePath.c_str());
                av_frame_unref(avFrame);
                av_packet_unref(avPacket);
                break;
            } else if (ret < 0) {
                LOGE("avcodec_receive_frame error while filePath is %s and ret is %d", mFilePath.c_str(), ret);
                av_frame_unref(avFrame);
                av_packet_unref(avPacket);
                return ret;
            }

            if (mIsVideoMode && avCodecContext->codec->type == AVMediaType::AVMEDIA_TYPE_VIDEO) {
                preProcessVideoFrame();
                av_frame_unref(avFrame);
                av_packet_unref(avPacket);
                return AV_OK;
            } else if (!mIsVideoMode && avCodecContext->codec->type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
                preProcessAudioFrame();
                av_frame_unref(avFrame);
                av_packet_unref(avPacket);
                return AV_OK;
            }

            av_frame_unref(avFrame);
        }

        av_packet_unref(avPacket);
    }

    return 0;
}

AVResult FFmpegDecoder::parseAVInfo() {
    if (mIsVideoMode) {
        mVideoWidth = avCodecContext->width;
        mVideoHeight = avCodecContext->height;
        mVideoPixelFormat = avCodecContext->pix_fmt;
        return mVideoWidth > 0 && mVideoHeight > 0 && mVideoPixelFormat != AV_PIX_FMT_NONE ? AV_OK : AV_ERROR;
    }

    return AV_OK;
}


AVResult FFmpegDecoder::preProcessVideoFrame() {
    if (avFrame->width != mVideoWidth && avFrame->height != mVideoHeight || avFrame->format != mVideoPixelFormat) {
        LOGE("preProcessVideoFrame error while filePath is %s", mFilePath.c_str());
        return AV_ERROR;
    }

    if (mAVRGBAImage == nullptr) {
        mAVRGBAImage = std::make_shared<AVRGBAImage>(mVideoWidth, mVideoHeight);
    }

    if (swsContext == nullptr) {
        swsContext = sws_getContext(mVideoWidth, mVideoHeight, mVideoPixelFormat,
                                    mAVRGBAImage->getWidth(), mAVRGBAImage->getHeight(), AV_PIX_FMT_RGBA,
                                    SWS_BILINEAR, NULL, NULL, NULL); // 缩放算法
    }

    uint8_t *dest[1] = { mAVRGBAImage->getRawPtr() };
    int dest_line_size[1] = { mAVRGBAImage->getWidth() * 4};
    sws_scale(swsContext,
              avFrame->data, avFrame->linesize, 0, avFrame->height,
              dest, dest_line_size);

    if (mVideoDecodeCallback) {
        mVideoDecodeCallback(avFrame->pts, mAVRGBAImage);
    }
    return 0;
}

AVResult FFmpegDecoder::preProcessAudioFrame() {
    return 0;
}