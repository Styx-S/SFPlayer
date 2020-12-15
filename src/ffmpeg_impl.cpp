#include "ffmpeg_impl.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


namespace sfplayer {
	FFMpegImpl::FFMpegImpl() {
		av_log_set_level(AV_LOG_DEBUG);
	}
	FFMpegImpl::~FFMpegImpl() {

	}


	bool FFMpegImpl::Play(std::string url) {
		int ret = avformat_open_input(&format_context_, url.c_str(), NULL, NULL);
		if (ret < 0) {
			printf("open input error\n");
			return false;
		}
		ret = avformat_find_stream_info(format_context_, NULL);
		if (ret < 0) {
			printf("find stream info error\n");
			return false;
		}

		AVStream *audioStream = nullptr, *videoStream = nullptr;
		for (int i = 0; i < format_context_->nb_streams; i++) {
			AVStream *currentStream = format_context_->streams[i];
			if (currentStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				audioStream = currentStream;
				audio_stream_index_ = i;
			}
			else if (currentStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStream = currentStream;
				video_stream_index_ = i;
			}
		}
		// 音频解码上下文初始化
		AVCodec *codec = avcodec_find_decoder(audioStream->codecpar->codec_id);
		if (!codec) {
			printf("find audio decoder error\n");
			return false;
		}
		audio_codec_context_ = avcodec_alloc_context3(codec);
		avcodec_parameters_to_context(audio_codec_context_, audioStream->codecpar);
		avcodec_open2(audio_codec_context_, codec, NULL);
		// 视频解码上下文初始化
		codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
		if (!codec) {
			printf("find video decoder error\n");
			return false;
		}
		video_codec_context_ = avcodec_alloc_context3(codec);
		avcodec_parameters_to_context(video_codec_context_, videoStream->codecpar);
		video_codec_context_->framerate = av_guess_frame_rate(format_context_, videoStream, NULL);
		avcodec_open2(video_codec_context_, codec, NULL);
		// 重采样：音频格式：按1024
		audio_swr_context_ = swr_alloc_set_opts(NULL,
			audio_codec_context_->channel_layout,
			AV_SAMPLE_FMT_S16P,
			1024,
			audio_codec_context_->channel_layout,
			audio_codec_context_->sample_fmt,
			audio_codec_context_->sample_rate,
			0,
			NULL);
		// 视频格式：转为YUV420P
		video_sws_context_ = sws_getCachedContext(video_sws_context_,
			video_codec_context_->width,
			video_codec_context_->height,
			video_codec_context_->pix_fmt,
			video_codec_context_->width,
			video_codec_context_->height,
			AV_PIX_FMT_YUV420P,
			SWS_BILINEAR,
			NULL, NULL, NULL);

		return true;	
	} 

	void FFMpegImpl::Start() {

		worker_ = std::make_shared<std::thread>(([this]() {
			while (running_) {
				AVPacket *packet = av_packet_alloc();
				av_read_frame(format_context_, packet);

				;
				AVFrame *frame = av_frame_alloc();
				if (packet->stream_index == audio_stream_index_) {
					// 音频解码
					avcodec_send_packet(audio_codec_context_, packet);
					avcodec_receive_frame(audio_codec_context_, frame);
					auto channels = audio_codec_context_->channels;
					int bufferSize = av_samples_get_buffer_size(NULL, channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
					AVFrame *outFrame = av_frame_alloc();
					outFrame->channel_layout = audio_codec_context_->channel_layout;
					outFrame->sample_rate = audio_codec_context_->sample_rate;
					outFrame->format = AV_SAMPLE_FMT_S16P;
					swr_convert_frame(audio_swr_context_, outFrame, frame);
					printf("linesize: %d", outFrame->linesize[0]);
					render_->pushAudioFrame(outFrame->data);
				} else if (packet->stream_index == video_stream_index_) {
					// 视频解码
					avcodec_send_packet(video_codec_context_, packet);
					avcodec_receive_frame(video_codec_context_, frame);
					uint8_t *dst_data[4];
					int dst_linesize[4];
					int ret = av_image_alloc(dst_data, dst_linesize, video_codec_context_->width, video_codec_context_->height, AV_PIX_FMT_YUV420P, 1);
					sws_scale(video_sws_context_, frame->data, frame->linesize, 0, video_codec_context_->height, dst_data, dst_linesize);
					render_->pushVideoFrame(dst_data, dst_linesize, video_codec_context_->width, video_codec_context_->height);
				}
				//av_packet_free(&packet);
				//av_frame_free(&frame);
			}
		}));
	}
	void FFMpegImpl::Stop() {
		running_ = false;
		// 等待线程正确退出
		worker_->join();
	}

	void FFMpegImpl::SetRenderOutput(std::shared_ptr<IRenderInterface> render) {
		render_ = render;
	}

}