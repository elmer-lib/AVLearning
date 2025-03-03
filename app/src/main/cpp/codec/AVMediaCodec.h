//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVMEDIACODEC_H
#define AVLEARNING_AVMEDIACODEC_H


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

static const AVOption ff_av_mediacodec_dec_options[] = {
        { NULL },
};

static const AVClass ff_av_mediacodec_dec_class = {
        .class_name = "ff_av_mediacodec_dec",
        .item_name  = av_default_item_name,
        .option     = ff_av_mediacodec_dec_options,
        .version    = LIBAVUTIL_VERSION_INT,
};

/**
 * FFAVMediaCodecDecContext
 */
struct FFAVMediaCodecDecContext {
};


static av_cold int av_mediacodec_decode_close(AVCodecContext *avctx) {
    FFAVMediaCodecDecContext *h = static_cast<FFAVMediaCodecDecContext *>(avctx->priv_data);
    int i;


    return 0;
}

static av_cold int av_mediacodec_decode_init(AVCodecContext *avctx) {
    FFAVMediaCodecDecContext *h = static_cast<FFAVMediaCodecDecContext *>(avctx->priv_data);
    int ret;

    return ret;
}

static av_cold int av_mediacodec_decode_decode(AVCodecContext *avctx, void *data,
                             int *got_frame, AVPacket *avpkt) {
    const uint8_t *buf              = avpkt->data;
    int buf_size                    = avpkt->size;
    FFAVMediaCodecDecContext *h     = static_cast<FFAVMediaCodecDecContext *>(avctx->priv_data);
    AVFrame *pict                   = static_cast<AVFrame *>(data);
    int buf_index;
    int ret;

//    h->flags = avctx->flags;
//    h->setup_finished = 0;
//    h->nb_slice_ctx_queued = 0;
}

static av_cold void av_mediacodec_decode_flush(AVCodecContext *avctx) {
    FFAVMediaCodecDecContext *h = static_cast<FFAVMediaCodecDecContext *>(avctx->priv_data);
    int i;
}

AVCodec ff_av_mediacodec_dec {
        .name           = "ff_av_mediacodec_dec",
        .long_name      = "av Android MediaCodec decoder",
        .type           = AVMEDIA_TYPE_VIDEO,
//        .id             = AV_CODEC_ID_H264,
//        .receive_frame  = av_mediacodec_receive_frame,
        .capabilities   = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_AVOID_PROBING | AV_CODEC_CAP_HARDWARE,
//        .caps_internal  = FF_CODEC_CAP_SETS_PKT_DTS,
//        .bsfs           = h264_mp4toannexb,
//        .hw_configs     = mediacodec_hw_configs,
        .priv_class     = &ff_av_mediacodec_dec_class,
        .wrapper_name   = "ff_av_mediacodec",
        .priv_data_size = sizeof(FFAVMediaCodecDecContext),
        .init           = av_mediacodec_decode_init,
        .decode         = av_mediacodec_decode_decode,
        .close          = av_mediacodec_decode_close,
        .flush          = av_mediacodec_decode_flush,
};


#endif //AVLEARNING_AVMEDIACODEC_H
