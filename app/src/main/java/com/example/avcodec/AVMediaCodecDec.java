package com.example.avcodec;

import android.annotation.SuppressLint;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

public class AVMediaCodecDec {
    private static final String TAG = "AVLearning-AVMediaCodecDec";

    // input parameters
    private String mInputFilePath = "";
    private int mOutputTextureId = -1;
    private Surface mOutputSurface = null;
    private AVDecodeFrameCallback mFrameCallback = null;
    private Boolean mIsStopRequested = false;

    // process parameters
    private MediaExtractor mMediaExtractor = null;
    private int mTargetTrackIndex = -1;
    private MediaCodec mMediaCodec = null;
    private final MediaCodec.BufferInfo mOutBufferInfo = new MediaCodec.BufferInfo();
    private int mVideoWidth = -1;
    private int mVideoHeight = -1;

    public int getVideoWidth() {
        synchronized (this) {
            return mVideoWidth;
        }
    }

    public int getVideoHeight() {
        synchronized (this) {
            return mVideoHeight;
        }
    }

    public String getInputVideoPath() {
        synchronized (this) {
            return mInputFilePath;
        }
    }

    public void init(String filePath, AVDecodeFrameCallback frameCallback) {
        synchronized (this) {
            mInputFilePath = filePath;
            mFrameCallback = frameCallback;

            // init extractor
            try {
                mMediaExtractor = new MediaExtractor();
                mMediaExtractor.setDataSource(mInputFilePath);
                int trackNum = mMediaExtractor.getTrackCount();
                for (int i = 0; i < trackNum; i++) {
                    MediaFormat format = mMediaExtractor.getTrackFormat(i);
                    String mime = format.getString(MediaFormat.KEY_MIME);
                    if (mime != null && mime.startsWith("video/")) {
                        mTargetTrackIndex = i;
                        break;
                    }
                }
                if (mTargetTrackIndex < 0)
                    throw new RuntimeException("No video track found in " + mInputFilePath);
                MediaFormat format = mMediaExtractor.getTrackFormat(mTargetTrackIndex);
                mVideoWidth = format.getInteger(MediaFormat.KEY_WIDTH);
                mVideoHeight = format.getInteger(MediaFormat.KEY_HEIGHT);

            } catch (IOException e) {
                throw new RuntimeException(e);
            }

            // init surfaceTexture
            // 创建一个数组来存储纹理ID
            int[] textureIds = new int[1]; // 创建一个数组，长度为1
            // 生成纹理ID
            GLES20.glGenTextures(1, textureIds, 0);
            // 纹理ID
            mOutputTextureId = textureIds[0];
            // 输出纹理ID
            Log.d("TextureID", "Generated Texture ID: " + mOutputTextureId);
            // 绑定纹理
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mOutputTextureId);
            // 设置纹理参数
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            // 创建 SurfaceTexture
            @SuppressLint("Recycle") SurfaceTexture surfaceTexture = new SurfaceTexture(mOutputTextureId);
            mOutputSurface = new Surface(surfaceTexture);
        }
    }

    public void unInit() {
        synchronized (this) {
            if (mMediaCodec != null) {
                mMediaCodec.stop();
                mMediaCodec.release();
                mMediaCodec = null;
            }
            if (mMediaExtractor != null) {
                mMediaExtractor.release();
                mMediaExtractor = null;
            }
            if (mOutputSurface != null) {
                mOutputSurface.release();
                mOutputSurface = null; // 防止悬空引用
            }
            GLES20.glDeleteTextures(1, new int[]{mOutputTextureId}, 0);
        }
    }

    public void requestStop() {
        synchronized (this) {
            mIsStopRequested = true;
        }
    }

    public void seekToTimeUs(int timeStamp) {
        synchronized (this) {
            if (mMediaExtractor == null || mMediaCodec == null)
                return;
            mMediaExtractor.seekTo(timeStamp, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
            mMediaCodec.flush();
        }
    }

    public void flush() {
        synchronized (this) {
            if (mMediaExtractor == null || mMediaCodec == null)
                return;
            mMediaCodec.flush();
        }
    }

    public void decodeNextFrame() {
        synchronized (this) {
            if (mMediaExtractor == null)
                return;

            if (mMediaCodec == null) {
                // Create a MediaCodec decoder, and configure it with the MediaFormat from the
                // extractor.  It's very important to use the format from the extractor because
                // it contains a copy of the CSD-0/CSD-1 codec-specific data chunks.
                MediaFormat format = mMediaExtractor.getTrackFormat(mTargetTrackIndex);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime == null)
                    return;

                try {
                    mMediaCodec = MediaCodec.createDecoderByType(mime);
                    mMediaCodec.configure(format, mOutputSurface, null, 0);
                    mMediaCodec.start();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }

            final int TIMEOUT_USEC = 10000;
            boolean isInputDone = false;
            while (true) {
                if (mIsStopRequested) {
                    Log.i(TAG, "decodeNextFrame: stop because mIsStopRequested is true");
                    break;
                }

                // Feed more data to the decoder.
                if (!isInputDone) {
                    int inputBufferIndex = mMediaCodec.dequeueInputBuffer(TIMEOUT_USEC);
                    if (inputBufferIndex >= 0) {
                        ByteBuffer inputBuffer = mMediaCodec.getInputBuffer(inputBufferIndex);
                        assert inputBuffer != null;
                        int chunkSize = mMediaExtractor.readSampleData(inputBuffer, 0);
                        if (chunkSize < 0) {
                            isInputDone = true;
                            mMediaCodec.queueInputBuffer(inputBufferIndex, 0, 0, 0L, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                            Log.d(TAG, "decodeNextFrame: send EOF to decode");
                        } else {
                            if (mMediaExtractor.getSampleTrackIndex() == mTargetTrackIndex) {
                                long presentTimeUs = mMediaExtractor.getSampleTime();
                                mMediaCodec.queueInputBuffer(inputBufferIndex, 0, chunkSize, presentTimeUs, 0);
                                mMediaExtractor.advance();
                            } else {
                                Log.e(TAG, "decodeNextFrame: getSampleTrackIndex is " + mMediaExtractor.getSampleTrackIndex() + " while mTargetTrackIndex is " + mTargetTrackIndex);
                            }
                        }
                    } else {
                        Log.e(TAG, "decodeNextFrame: inputBufferIndex:" + inputBufferIndex + " is error");
                    }
                }

                int decStatus = mMediaCodec.dequeueOutputBuffer(mOutBufferInfo, TIMEOUT_USEC);
                if (decStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    Log.d(TAG, "decodeNextFrame: no output frame now");
                } else if (decStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                    // not important for us, since we're using Surface
                    Log.d(TAG, "decodeNextFrame: output buffers changed");
                } else if (decStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    MediaFormat format = mMediaCodec.getOutputFormat();
                    Log.w(TAG, "decodeNextFrame: new media format is " + format);
                } else if (decStatus < 0) {
                    throw new RuntimeException("unexpected result from decoder.dequeueOutputBuffer: " + decStatus);
                } else {
                    if ((mOutBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        if (mFrameCallback != null)
                            mFrameCallback.notifyEOF();
                    }

                    boolean doRender = mOutBufferInfo.size != 0;
                    // As soon as we call releaseOutputBuffer, the buffer will be forwarded
                    // to SurfaceTexture to convert to a texture.  We can't control when it
                    // appears on-screen, but we can manage the pace at which we release
                    // the buffers.
                    if (doRender && mFrameCallback != null)
                        mFrameCallback.preRenderToSurface();
                    mMediaCodec.releaseOutputBuffer(decStatus, doRender);
                    if (doRender && mFrameCallback != null)
                        mFrameCallback.postRenderToSurface();
                    break;
                }
            }
        }
    }

    public interface AVDecodeFrameCallback {
        void preRenderToSurface();

        void postRenderToSurface();

        void notifyEOF();
    }
}
