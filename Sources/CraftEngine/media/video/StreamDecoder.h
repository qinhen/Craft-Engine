#pragma once
#ifndef CRAFT_ENGINE_MEDIA_STREAM_DECODER_H_
#define CRAFT_ENGINE_MEDIA_STREAM_DECODER_H_

#include "../../3rdparty/ThirdPartyImportFFmpeg.h"
#include "../../3rdparty/ThirdPartyImportLibyuv.h"

#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <mutex>
#include <atomic>


namespace CraftEngine
{
	namespace Media
	{



		struct StreamMediaFile
		{
			const char* m_filename;

			AVFormatContext* m_formatContex = nullptr;

			AVCodecContext* m_videoCodecContext = nullptr;
			AVCodecContext* m_audioCodecContext = nullptr;
			AVPacket* m_packet = nullptr;
			AVFrame* m_decodedFrame = nullptr;
			AVFrame* m_tempFrame = nullptr;
			uint8_t* m_tempFrameBuffer = nullptr;

			AVSampleFormat m_audioSampleFormat;

			AVFrame* m_rgbaFrame = nullptr;
			SwsContext* m_swsContext = nullptr;
			SwrContext* m_swrContext = nullptr;

			std::atomic_bool m_shouldInterrupt = false;

			int m_videoStreamIndex = -1;
			int m_audioStreamIndex = -1;

			bool m_isValid = false;

			struct AudioFrame
			{
				uint8_t* data;
				uint32_t data_size;
				uint32_t channal_count;
				uint32_t sample_count;
				uint32_t sample_rate;
				double present_timestamp;
				bool used_flag;
			};

			struct VideoFrame
			{
				uint8_t* data;
				uint32_t data_size;
				uint32_t width;
				uint32_t height;
				double present_timestamp;
				bool used_flag;
			};
			std::list<AudioFrame*> m_audioFrames;
			std::list<VideoFrame*> m_videoFrames;
			std::list<VideoFrame*> m_videoFrameBuffers;

			std::mutex m_audioMutex;
			std::mutex m_videoMutex;

			double m_duration;

			void clear()
			{
				if (m_formatContex != nullptr)
					avformat_close_input(&m_formatContex);

				if (m_audioCodecContext != nullptr)
					avcodec_free_context(&m_audioCodecContext);
				if (m_videoCodecContext != nullptr)
					avcodec_free_context(&m_videoCodecContext);

				if (m_packet != nullptr)
					av_packet_free(&m_packet);
				if (m_decodedFrame != nullptr)
					av_frame_free(&m_decodedFrame);
				if (m_tempFrame != nullptr)
					av_frame_free(&m_tempFrame);
				if (m_tempFrameBuffer != nullptr)
					delete[] m_tempFrameBuffer;
				
				m_videoStreamIndex = -1;
				m_audioStreamIndex = -1;


				m_formatContex = nullptr;
				m_videoCodecContext = nullptr;
				m_audioCodecContext = nullptr;
				m_packet = nullptr;
				m_decodedFrame = nullptr;
				m_tempFrame = nullptr;
				m_tempFrameBuffer = nullptr;

				if (m_rgbaFrame != nullptr)
					av_frame_free(&m_rgbaFrame);
				m_rgbaFrame = nullptr;
				if (m_swsContext != nullptr)
					sws_freeContext(m_swsContext);
				m_swsContext = nullptr;


				if (m_swrContext != nullptr)
					swr_free(&m_swrContext);
				m_swrContext = nullptr;


				{
					dequeueVideoFrame(m_videoFrames.size());
				}

				{
					std::lock_guard<std::mutex> lock(m_videoMutex);
					for (auto& f : m_videoFrameBuffers)
					{
						freeVideoFrame(&f);
					}
					m_videoFrameBuffers.clear();
				}

				{
					std::lock_guard<std::mutex> lock(m_audioMutex);
					for (auto& f : m_audioFrames)
					{
						freeAudioFrame(&f);
					}
					m_audioFrames.clear();
				}

				m_isValid = false;
			}




			static AudioFrame* allocAudioFrame(uint32_t channal_count, uint32_t sample_count, uint32_t sample_rate)
			{
				AudioFrame* frame = new AudioFrame;
				frame->data_size = sizeof(short) * channal_count * sample_count;
				frame->data = (uint8_t*)malloc(frame->data_size);
				frame->channal_count = channal_count;
				frame->sample_count = sample_count;
				frame->sample_rate = sample_rate;
				return frame;
			}

			static void freeAudioFrame(AudioFrame** ppFrame)
			{
				if ((*ppFrame)->data != nullptr)
					delete[](*ppFrame)->data;
				delete (*ppFrame);
				(*ppFrame) = nullptr;
			}

			static VideoFrame* allocVideoFrame(uint32_t width, uint32_t height)
			{
				VideoFrame* frame = new VideoFrame;
				frame->data_size = 4 * width * height;
				frame->data = (uint8_t*)malloc(frame->data_size);
				frame->width = width;
				frame->height = height;
				return frame;
			}

			static void freeVideoFrame(VideoFrame** ppFrame)
			{
				if ((*ppFrame)->data != nullptr)
					delete[](*ppFrame)->data;
				delete (*ppFrame);
				(*ppFrame) = nullptr;
			}

		public:

			bool isValid()const
			{
				return m_isValid;
			}

			StreamMediaFile()
			{

			}

			~StreamMediaFile()
			{
				clear();
			}

			int getSampleRate()const { if (!isValid())return 0; return m_formatContex->streams[m_audioStreamIndex]->codecpar->sample_rate; }
			int getFrameRate()const { if (!isValid())return 0; return av_q2d(m_formatContex->streams[m_videoStreamIndex]->r_frame_rate); }

			bool haveAudioStream()const { if (!isValid())return false; return m_audioStreamIndex != -1; }
			bool haveVideoStream()const { if (!isValid())return false; return m_videoStreamIndex != -1; }

			int getVideoWidth()const { if (!isValid())return 0; return m_formatContex->streams[m_videoStreamIndex]->codecpar->width; }
			int getVideoHeight()const { if (!isValid())return 0; return m_formatContex->streams[m_videoStreamIndex]->codecpar->height; }

			double getDuration()const { if (!isValid())return 0; return m_duration; }


			void seekFrame(double time)
			{

				//int64_t timestamp = time / av_q2d(m_formatContex->streams[m_audioStreamIndex]->time_base);
				//av_seek_frame(m_formatContex, m_audioStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD);

				if (haveVideoStream())
				{
					int64_t timestamp = time / av_q2d(m_formatContex->streams[m_videoStreamIndex]->time_base);
					av_seek_frame(m_formatContex, m_videoStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD);
					avcodec_flush_buffers(m_audioCodecContext);
					avcodec_flush_buffers(m_videoCodecContext);
				}
				else
				{
					int64_t timestamp = time / av_q2d(m_formatContex->streams[m_audioStreamIndex]->time_base);
					av_seek_frame(m_formatContex, m_audioStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD);
					avcodec_flush_buffers(m_audioCodecContext);
				}

			}

			std::vector<uint8_t> getHeaderImage()
			{
				return std::vector<uint8_t>();
				if (!isValid())
					return std::vector<uint8_t>();

				if (m_formatContex->iformat->read_header(m_formatContex) < 0) {
					printf("No header format");
					return std::vector<uint8_t>();
				}
				for (int i = 0; i < m_formatContex->nb_streams; i++) {
					if (m_formatContex->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
						AVPacket pkt = m_formatContex->streams[i]->attached_pic;
						std::vector<uint8_t> data(pkt.size);
						memcpy_s(data.data(), pkt.size, (uint8_t*)pkt.data, pkt.size);
						return data;
					}
				}
				return std::vector<uint8_t>();
			}


			bool openFile(const std::string& filename)
			{
				clear();

				m_formatContex = avformat_alloc_context();
				/* 2. 打开流 */
				if (avformat_open_input(&m_formatContex, filename.c_str(), NULL, NULL) != 0)
				{
					printf("Couldn't open input stream.\n");
					clear();
					return false;
				}
				if (avformat_find_stream_info(m_formatContex, NULL) < 0) {
					printf("could not find stream information");
					clear();
					return false;
				}

				for (int i = 0; i < m_formatContex->nb_streams; i++)
					if (m_formatContex->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						m_videoStreamIndex = i;
						switch (m_formatContex->streams[i]->codecpar->codec_id)
						{
						case AV_CODEC_ID_H264:
						case AV_CODEC_ID_H265:
						case AV_CODEC_ID_MPEG4:
							break;
						default:
							m_videoStreamIndex = -1;
							break;
						}
						if (m_videoStreamIndex != -1)
							break;
					}

				for (int i = 0; i < m_formatContex->nb_streams; i++)
					if (m_formatContex->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
					{
						m_audioStreamIndex = i;
						break;
					}

				if (m_audioStreamIndex == -1)
				{
					clear();
					return false;
				}


				if (m_videoStreamIndex != -1)
				{
					AVStream* video_stream = m_formatContex->streams[m_videoStreamIndex];
					AVCodecParameters* pCodecPar = video_stream->codecpar;

					// 查找解码器
					const AVCodec* m_codec = avcodec_find_decoder(pCodecPar->codec_id);
					std::cout << "Codec: " << m_codec->name << std::endl;
					if (!m_codec) {
						fprintf(stderr, "Codec not found\n");
						clear();
						return false;
					}
					// 分配解码器上下文
					m_videoCodecContext = avcodec_alloc_context3(m_codec);
					if (!m_videoCodecContext) {
						fprintf(stderr, "Could not allocate video codec context\n");
						clear();
						return false;
					}
					if (avcodec_parameters_to_context(m_videoCodecContext, pCodecPar) < 0) {
						fprintf(stderr, "Didn't parameters to contex.");
						clear();
						return false;
					}

					// 打开解码器
					if (avcodec_open2(m_videoCodecContext, m_codec, NULL) < 0) {
						fprintf(stderr, "Could not open codec\n");
						clear();
						return false;
					}

				}

				if (m_videoStreamIndex != -1)
				{
					m_rgbaFrame = av_frame_alloc();
					if (!m_rgbaFrame) {
						fprintf(stderr, "Could not allocate video frame - 0\n");
						clear();
						return false;
					}
					m_swsContext = sws_getContext(
						m_formatContex->streams[m_videoStreamIndex]->codecpar->width,
						m_formatContex->streams[m_videoStreamIndex]->codecpar->height,
						(AVPixelFormat)m_formatContex->streams[m_videoStreamIndex]->codecpar->format,
						m_formatContex->streams[m_videoStreamIndex]->codecpar->width,
						m_formatContex->streams[m_videoStreamIndex]->codecpar->height,
						AV_PIX_FMT_RGBA,
						SWS_BICUBIC, NULL, NULL, NULL);

					m_tempFrame = av_frame_alloc();
					if (!m_tempFrame) {
						fprintf(stderr, "Could not allocate video frame - 1\n");
						clear();
						return false;
					}
					int temp_frame_data_size = av_image_get_buffer_size(
						(AVPixelFormat)m_formatContex->streams[m_videoStreamIndex]->codecpar->format,
						m_formatContex->streams[m_videoStreamIndex]->codecpar->width,
						m_formatContex->streams[m_videoStreamIndex]->codecpar->height, 1);
					m_tempFrameBuffer = new uint8_t[temp_frame_data_size];
				}


				if (m_audioStreamIndex != -1)
				{
					AVStream* audio_stream = m_formatContex->streams[m_audioStreamIndex];
					AVCodecParameters* pCodecPar = audio_stream->codecpar;

					// 查找解码器
					const AVCodec* m_codec = avcodec_find_decoder(pCodecPar->codec_id);
					std::cout << "Codec: " << m_codec->name << std::endl;
					if (!m_codec) {
						fprintf(stderr, "Codec not found\n");
						clear();
						return false;
					}
					// 分配解码器上下文
					m_audioCodecContext = avcodec_alloc_context3(m_codec);
					if (!m_audioCodecContext) {
						fprintf(stderr, "Could not allocate audio codec context\n");
						clear();
						return false;
					}
					if (avcodec_parameters_to_context(m_audioCodecContext, pCodecPar) < 0) {
						fprintf(stderr, "Didn't parameters to contex.");
						clear();
						return false;
					}
					// 打开解码器
					if (avcodec_open2(m_audioCodecContext, m_codec, NULL) < 0) {
						fprintf(stderr, "Could not open codec\n");
						clear();
						return false;
					}
				}

				if (m_audioStreamIndex != -1)
				{
					m_swrContext = swr_alloc_set_opts(nullptr,
						AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, m_audioCodecContext->sample_rate,
						m_audioCodecContext->channel_layout, m_audioCodecContext->sample_fmt, m_audioCodecContext->sample_rate,
						0, nullptr
					);
					swr_init(m_swrContext);
				}


				// 包
				m_packet = av_packet_alloc();
				if (!m_packet) {
					fprintf(stderr, "Could not allocate audio codec context\n");
					clear();
					return false;
				}
				// 音频流帧容器
				m_decodedFrame = av_frame_alloc();
				if (!m_decodedFrame) {
					fprintf(stderr, "Could not allocate audio frame\n");
					clear();
					return false;
				}


				m_isValid = true;
				m_audioSampleFormat = m_audioCodecContext->sample_fmt;


				m_duration = av_q2d(m_formatContex->streams[m_audioStreamIndex]->time_base) * m_formatContex->streams[m_audioStreamIndex]->duration;
				if (m_duration <= 0)
				{
					std::ifstream ofs(filename, std::ios::binary);
					if (!ofs.is_open())
					{
						clear();
						return false;
					}
					ofs.seekg(0, std::ios::end);
					int64_t filesize = ofs.tellg();
					std::cout << "filesize = " << filesize << std::endl;
					m_duration = double(filesize * 8) / m_formatContex->bit_rate;
				}

				std::cout << "duration = " << m_duration << std::endl;
				return true;
			}


			int getDuration()
			{
				return m_duration;
			}

			class PcmDataConvertor
			{
				union float_int_cov
				{
					int i;
					float f;
				};

			public:

				static int32_t to_int32(float val)
				{
					float_int_cov temp;
					constexpr const int shift = 31;
					temp.f = val + (1.5f * (1 << (23 - shift)) + 0.5f / (1 << shift));
					temp.i -= (((150 - shift) << 23) + (1 << 22));
					return temp.i;
				}

				static int16_t to_int16(float val)
				{
					float_int_cov temp;
					constexpr const int shift = 15;
					temp.f = val + (1.5f * (1 << (23 - shift)) + 0.5f / (1 << shift));
					temp.i -= (((150 - shift) << 23) + (1 << 22));
					if ((unsigned int)(temp.i + 32768) > 65535)
						temp.i = temp.i < 0 ? -32768 : 32767;
					return temp.i;
				}

				static int16_t to_int16(int32_t val)
				{
					return (int16_t)(val >> 16);
				}

				static int16_t to_int16(int8_t val)
				{
					return (int16_t)(val) << 8;
				}
			};


			void interupt()
			{
				m_shouldInterrupt = true;
			}



			bool decode(uint32_t req, CraftEngine::core::Thread* processThread)
			{
				m_shouldInterrupt = false;

				if (haveVideoStream())
				{
					bool should_dequeue_video = false;
					{
						std::lock_guard<std::mutex> lock(m_videoMutex);
						if (m_videoFrames.size() >= req)
							should_dequeue_video = true;
					}
					if (should_dequeue_video)
					{
						std::lock_guard<std::mutex> lock(m_audioMutex);
						if (m_audioFrames.size() == 0)
						{
							std::cout << "discard one frame" << std::endl;
							dequeueVideoFrame(1);
						}
					}
				}

				int eof = 0;
				while (1)
				{
					if (m_shouldInterrupt)
						break;

					eof = av_read_frame(m_formatContex, m_packet);
					if (eof < 0)
						break;

					if (m_packet->stream_index == m_videoStreamIndex)
					{
						if (avcodec_send_packet(m_videoCodecContext, m_packet) == 0)
						{
							while (1)
							{
								if (processThread != nullptr)
								{
									if (avcodec_receive_frame(m_videoCodecContext, m_decodedFrame) != 0)
										break;
									processThread->wait();

									av_image_fill_arrays(m_tempFrame->data, m_tempFrame->linesize, m_tempFrameBuffer, (AVPixelFormat)m_decodedFrame->format, getVideoWidth(), getVideoHeight(), 1);
									av_frame_copy_props(m_tempFrame, m_decodedFrame);
									m_tempFrame->width = m_decodedFrame->width;
									m_tempFrame->height = m_decodedFrame->height;
									av_image_copy(m_tempFrame->data, m_tempFrame->linesize, (const uint8_t**)m_decodedFrame->data, m_decodedFrame->linesize, (AVPixelFormat)m_decodedFrame->format, getVideoWidth(), getVideoHeight());
									
									CraftEngine::core::Command<void> cmd;
									cmd.wrap(this, &StreamMediaFile::processVideoFrame, m_tempFrame);
									processThread->push(cmd);
								}
								else
								{
									if (avcodec_receive_frame(m_videoCodecContext, m_decodedFrame) != 0)
										break;
									processVideoFrame(m_decodedFrame);
								}
							}
						}
					}
					else if (m_packet->stream_index == m_audioStreamIndex)
					{
						if (avcodec_send_packet(m_audioCodecContext, m_packet) == 0)
						{
							while (1)
							{
								if (avcodec_receive_frame(m_audioCodecContext, m_decodedFrame) != 0)
									break;
								processAudioFrame(m_decodedFrame);
							}
						}		
					}
					av_packet_unref(m_packet);

					if (haveVideoStream())
					{
						std::lock_guard<std::mutex> lock(m_videoMutex);
						if (m_videoFrames.size() >= req - 1)
							break;
					}
					else
					{
						std::lock_guard<std::mutex> lock(m_audioMutex);
						if (m_audioFrames.size() >= req)
							break;
					}
				}

				if (processThread != nullptr)
					processThread->wait();
				if (eof < 0)
				{
					std::cout << "eof" << std::endl;
					return false;
				}
				else
					return true;
			}




	
			void dequeueVideoFrame(int count)
			{
				std::lock_guard<std::mutex> lock(m_videoMutex);
				int nb = count > m_videoFrames.size() ? m_videoFrames.size() : count;
				for (int i = 0; i < count; i++)
				{
					if (m_videoFrameBuffers.size() < 32)
						m_videoFrameBuffers.push_back(m_videoFrames.front());
					else
						freeVideoFrame(&m_videoFrames.front());
					m_videoFrames.pop_front();
				}
			}

			void dequeueAudioFrame(int count)
			{
				std::lock_guard<std::mutex> lock(m_audioMutex);
				int nb = count > m_audioFrames.size() ? m_audioFrames.size() : count;
				for (int i = 0; i < nb; i++)
				{
					freeAudioFrame(&m_audioFrames.front());
					m_audioFrames.pop_front();
				}
			}


			void dequeueAllVideoFrame()
			{
				std::lock_guard<std::mutex> lock(m_videoMutex);
				int count = m_videoFrames.size();
				for (int i = 0; i < count; i++)
				{
					if (m_videoFrameBuffers.size() < 32)
						m_videoFrameBuffers.push_back(m_videoFrames.front());
					else
						freeVideoFrame(&m_videoFrames.front());
					m_videoFrames.pop_front();
				}
			}

			void dequeueAllAudioFrame()
			{
				std::lock_guard<std::mutex> lock(m_audioMutex);
				int count = m_audioFrames.size();
				for (int i = 0; i < count; i++)
				{
					freeAudioFrame(&m_audioFrames.front());
					m_audioFrames.pop_front();
				}
			}


		private:
			void processVideoFrame(AVFrame* avfrmae)
			{
				VideoFrame* frame = nullptr;
				{
					std::lock_guard<std::mutex> lock(m_videoMutex);
					if (m_videoFrameBuffers.size() != 0)
					{
						frame = m_videoFrameBuffers.front();
						m_videoFrameBuffers.pop_front();
					}
					else
					{
						frame = allocVideoFrame(avfrmae->width, avfrmae->height);
					}
				}

				AVPixelFormat format = (AVPixelFormat)avfrmae->format;
				frame->used_flag = false;
				frame->present_timestamp = av_q2d(m_formatContex->streams[m_videoStreamIndex]->time_base) * avfrmae->pts;
				switch (format)
				{
				case AV_PIX_FMT_YUV420P:
				{
					libyuv::I420ToABGR(
						avfrmae->data[0], avfrmae->linesize[0],
						avfrmae->data[1], avfrmae->linesize[1],
						avfrmae->data[2], avfrmae->linesize[2],
						frame->data, frame->width * 4,
						frame->width, frame->height
					);
					break;
				}
				case AV_PIX_FMT_YUV444P:
				{
					libyuv::I444ToABGR(
						avfrmae->data[0], avfrmae->linesize[0],
						avfrmae->data[1], avfrmae->linesize[1],
						avfrmae->data[2], avfrmae->linesize[2],
						frame->data, frame->width * 4,
						frame->width, frame->height
					);
					break;
				}
				default:
				{
					av_image_fill_arrays(m_rgbaFrame->data, m_rgbaFrame->linesize, frame->data, AV_PIX_FMT_RGBA, frame->width, frame->height, 4);
					auto slice = sws_scale(m_swsContext, (const uint8_t* const*)avfrmae->data, avfrmae->linesize, 0, frame->height, m_rgbaFrame->data, m_rgbaFrame->linesize);
					if (slice <= 0)
					{
						freeVideoFrame(&frame);
						std::cout << "Unrecorngized Format = " << av_get_pix_fmt_name(format) << std::endl;
					}
				}
				}
				if (frame != nullptr)
				{
					std::lock_guard<std::mutex> lock(m_videoMutex);
					m_videoFrames.push_back(frame);
				}
			}


			void processAudioFrame(AVFrame* avfrmae)
			{
				AudioFrame* frame = allocAudioFrame(avfrmae->channels, avfrmae->nb_samples, avfrmae->sample_rate);
				frame->used_flag = false;
				frame->present_timestamp = av_q2d(m_formatContex->streams[m_audioStreamIndex]->time_base) * avfrmae->pts;
				AVSampleFormat format = (AVSampleFormat)avfrmae->format;

				if (avfrmae->channels != 2)
				{
					swr_convert(m_swrContext, &frame->data, frame->sample_count, (const uint8_t**)avfrmae->data, avfrmae->nb_samples);
				}
				else
				{
					switch (format)
					{
					case AV_SAMPLE_FMT_S16P:
					{
						for (int i = 0; i < frame->sample_count; i++)
							for (int j = 0; j < frame->channal_count; j++)
								((short*)(frame->data))[i * frame->channal_count + j] = (((int16_t*)avfrmae->data[j])[i]);
						break;
					}
					case AV_SAMPLE_FMT_S16:
					{
						memcpy(frame->data, avfrmae->data[0], frame->sample_count * sizeof(short) * frame->channal_count);
						break;
					}
					case AV_SAMPLE_FMT_FLT:
					{
						for (int i = 0; i < frame->sample_count; i++)
							for (int j = 0; j < frame->channal_count; j++)
								((short*)(frame->data))[i * frame->channal_count + j] = PcmDataConvertor::to_int16((((float*)avfrmae->data[0])[i * frame->channal_count + j]));
						break;
					}
					case AV_SAMPLE_FMT_FLTP:
					{
						for (int i = 0; i < frame->sample_count; i++)
							for (int j = 0; j < frame->channal_count; j++)
								((short*)(frame->data))[i * frame->channal_count + j] = PcmDataConvertor::to_int16((((float*)avfrmae->data[j])[i]));
						break;
					}
					case AV_SAMPLE_FMT_S32:
					{
						for (int i = 0; i < frame->sample_count; i++)
							for (int j = 0; j < frame->channal_count; j++)
								((short*)(frame->data))[i * frame->channal_count + j] = PcmDataConvertor::to_int16((((int32_t*)avfrmae->data[0])[i * frame->channal_count + j]));
						break;
					}
					case AV_SAMPLE_FMT_S32P:
					{
						for (int i = 0; i < frame->sample_count; i++)
							for (int j = 0; j < frame->channal_count; j++)
								((short*)(frame->data))[i * frame->channal_count + j] = PcmDataConvertor::to_int16((((int32_t*)avfrmae->data[j])[i]));
						break;
					}
					case AV_SAMPLE_FMT_NONE:
					{
						if (av_get_bytes_per_sample(format) == 2)
						{
							memcpy(frame->data, avfrmae->data[0], frame->sample_count * sizeof(short) * frame->channal_count);
							break;
						}
						else if (av_get_bytes_per_sample(format) == 4)
						{
							for (int i = 0; i < frame->sample_count; i++)
								for (int j = 0; j < frame->channal_count; j++)
									((short*)(frame->data))[i * frame->channal_count + j] = PcmDataConvertor::to_int16((((int32_t*)avfrmae->data[0])[i * frame->channal_count + j]));
						}
					}
					default:
					{
						std::cout << "Unrecorngized Format = " << av_get_sample_fmt_name(format) << std::endl;
						freeAudioFrame(&frame);
					}
					}
				}
				if (frame != nullptr)
				{
					std::lock_guard<std::mutex> lock(m_audioMutex);
					m_audioFrames.push_back(frame);
				}
			}


		};



	}
}
#endif // !CRAFT_ENGINE_MEDIA_STREAM_DECODER_H_