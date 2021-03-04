#pragma once
#ifndef CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_
#define CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_
/*
* 
* 
* 
*/
#include "../../advance/openal/OpenALApi.h"
#include "../../media/video/StreamDecoder.h"

#include "../widgets/Widget.h"

namespace CraftEngine
{
	namespace gui
	{



		class MediaPlayerWidget :public Widget
		{
		private:

			CraftEngine::openal::AudioStreamPlayer m_audioPlayer;
			CraftEngine::Media::StreamMediaFile m_file;
			CraftEngine::core::Thread* m_workThread;
			CraftEngine::core::Thread* m_processThread;

			std::mutex m_decodeMutex;
			double m_timestamp = 0.0;
			double m_videoFrameTimestamp = 0.0;

			enum PlayState
			{
				player_state_stop,
				player_state_playing,
				player_state_pause,
			};
			PlayState m_playerState;

			std::atomic_bool m_isDecoding = false;
			bool m_isEof = false;

			HandleImage m_frameImage = HandleImage(nullptr);

			bool should_decode()
			{
				if (!mediaIsPlaying())
					return false;
				if (m_isDecoding)
				{
					return false;
				}
				if (m_isEof)
				{
					mediaSetStop();
					return false;
				}

				if (m_file.haveVideoStream())
				{
					std::lock_guard<std::mutex> lock(m_file.m_videoMutex);
					if (m_file.m_videoFrames.size() < 31)
						return true;
					auto last_bufs_count = m_audioPlayer.getCurQueuedBuffer();
					if (last_bufs_count < 6)
						return true;
					else
						return false;
				}
				else
				{
					auto last_bufs_count = m_audioPlayer.getCurQueuedBuffer();
					if (last_bufs_count < 24)
						return true;
					else
						return false;
				}
			}


			void decode()
			{
				if (m_decodeMutex.try_lock())
				{
					if (!this->m_file.decode(32, m_processThread))
						this->m_isEof = true;
					m_decodeMutex.unlock();
				}
				this->m_isDecoding = false;
			};

			int mediaGetSampleRate()
			{
				return m_file.getSampleRate();
			}
			int mediaGetFrameRate()
			{
				return m_file.getFrameRate();
			}

			double getCurTimestamp()
			{
				return m_timestamp;
			}

			void remove_invalid_frame()
			{
				auto used_buffer_count = m_audioPlayer.getCurUsedBuffer();
				std::vector<openal::Buffer> bufs;
				{
					std::lock_guard<std::mutex> lock(m_file.m_audioMutex);
					update_timestamp();
					for (auto f : m_file.m_audioFrames)
					{
						if (f->used_flag == false)
						{
							f->used_flag = true;
							openal::Buffer buf = openal::createBuffer();
							buf.setDataInt16((short*)f->data, f->sample_count, f->sample_rate);
							bufs.push_back(buf);
						}
					}
				}
				if (bufs.size() > 0)
					m_audioPlayer.enqueue_lock(bufs);

				if (used_buffer_count == 0)
					return;

				auto audio_removed_count = m_audioPlayer.removeInvalidStreamBuffer();
				if (audio_removed_count <= 0)
					return;
				m_file.dequeueAudioFrame(audio_removed_count);

				auto video_remove_count = 0;
				{
					std::lock_guard<std::mutex> lock(m_file.m_videoMutex);

					if (m_file.m_videoFrames.size() == 0)
						return;
					else if (m_file.m_videoFrames.size() > 32)
					{
						video_remove_count = m_file.m_videoFrames.size() - 32;
					}
					else
					{
						auto it = m_file.m_videoFrames.begin();
						for (int i = 0; i < m_file.m_videoFrames.size() - 1; i++, it++)
						{
							if ((*it)->present_timestamp < m_timestamp)
								video_remove_count++;
							else
								break;
						}
					}

				}
				m_file.dequeueVideoFrame(video_remove_count);

			}


			void update_timestamp()
			{
				if (m_file.m_audioFrames.size() == 0)
					return;
				auto it = m_file.m_audioFrames.begin();
				m_timestamp = (*it)->present_timestamp + ((double)m_audioPlayer.getOffset()) / mediaGetSampleRate();
			}


			void update_video_frame()
			{
				int i = 0;
				auto it = m_file.m_videoFrames.begin();
				bool have_frame = false;
				{
					std::lock_guard<std::mutex> lock(m_file.m_videoMutex);
					it = m_file.m_videoFrames.begin();
					if (m_file.m_videoFrames.size() != 0)
					{
						for (i = 0; i < m_file.m_videoFrames.size() - 1; i++, it++)
						{
							if ((*it)->present_timestamp >= m_timestamp)
								break;
							else if ((*it)->used_flag == false)
								break;
						}
						have_frame = true;
					}
					if (have_frame && (*it)->present_timestamp != m_videoFrameTimestamp)
					{
						(*it)->used_flag = true;
						//std::cout << "set one frame" << std::endl;
						m_videoFrameTimestamp = (*it)->present_timestamp;
						GuiRenderSystem::updateImage(m_frameImage, (*it)->data, (*it)->data_size, (*it)->width, (*it)->height);
					}
				}
				if (i > 0)
				{
					m_file.dequeueVideoFrame(i);
				}
			}


			void process()
			{
				if (!mediaIsPlaying())
					return;

				remove_invalid_frame();

				// Ω‚¬Î
				if (should_decode())
				{
					m_isDecoding = true;
					CraftEngine::core::Command<void> cmd;
					cmd.wrap(this, &MediaPlayerWidget::decode);
					m_workThread->push(cmd);
				}

				// º”‘ÿ ”∆µ÷°
				if (m_file.haveVideoStream())
				{
					update_video_frame();
				}

				// ª÷∏¥≤•∑≈
				if (mediaIsPlaying() && !m_audioPlayer.isPlaying() && !m_isDecoding)
				{
					std::cout << "set play..." << std::endl;
					m_audioPlayer.play();
				}

			}
		public:

			MediaPlayerWidget(const Rect& rect, Widget* p) : Widget(rect, p)
			{
				m_workThread = new CraftEngine::core::Thread();
				m_processThread = new CraftEngine::core::Thread();
				m_audioPlayer.init();
			}

			MediaPlayerWidget(Widget* p) : Widget(p)
			{
				m_workThread = new CraftEngine::core::Thread();
				m_processThread = new CraftEngine::core::Thread();
				m_audioPlayer.init();
			}

			virtual ~MediaPlayerWidget()
			{
				if (m_frameImage != nullptr)
				{
					GuiRenderSystem::deleteImage(m_frameImage);
				}
				m_frameImage = HandleImage(nullptr);
			}


			void clear()
			{
				m_audioPlayer.stop_lock();
				std::lock_guard<std::mutex> lock(m_decodeMutex);
				m_isEof = false;
				if (m_frameImage != nullptr)
				{
					GuiRenderSystem::deleteImage(m_frameImage);
					m_frameImage = HandleImage(nullptr);
				}
				m_timestamp = 0;
				m_file.clear();
			}


			double mediaGetDuration()
			{
				return m_file.getDuration();
			}

			double mediaGetTimestamp()
			{
				return m_timestamp;
			}

			void mediaSeekFrame(double time)
			{
				if (m_file.isValid())
				{
					m_isEof = true;
					m_audioPlayer.stop_lock();
					m_playerState = player_state_stop;
					m_file.interupt();
					std::lock_guard<std::mutex> lock(m_decodeMutex);
					m_file.dequeueAllAudioFrame();
					if (m_file.haveVideoStream())
						m_file.dequeueAllVideoFrame();
					m_file.seekFrame(time);
					m_timestamp = time;
					m_isEof = false;
					mediaSetPlay();
				}
			}


			bool mediaSetFile(const std::string& url)
			{
				mediaSetStop();

				m_file.openFile(url);
				if (!m_file.isValid())
					return false;

				if (m_file.haveVideoStream())
				{
					void* data = malloc(4 * m_file.getVideoWidth() * m_file.getVideoHeight());
					m_frameImage = GuiRenderSystem::createImage(data, 4 * m_file.getVideoWidth() * m_file.getVideoHeight(), m_file.getVideoWidth(), m_file.getVideoHeight());
					free(data);
				}

				return true;
			}

			bool mediaIsStop() { return m_playerState == player_state_stop; }
			bool mediaIsPaused() { return m_playerState == player_state_pause; }
			bool mediaIsPlaying() { return m_playerState == player_state_playing; }


			craft_engine_gui_signal(mediaPlay, void(void));
			craft_engine_gui_signal(mediaPause, void(void));
			craft_engine_gui_signal(mediaStop, void(void));


			bool mediaSetStop()
			{
				if (m_file.isValid())
				{
					clear();
					m_playerState = player_state_stop;
					craft_engine_gui_emit(mediaStop, );
				}
				return true;
			}

			bool mediaSetPaused()
			{
				if (m_file.isValid())
				{
					m_audioPlayer.pause();
					m_playerState = player_state_pause;
					craft_engine_gui_emit(mediaPause, );
					return true;
				}
				else
				{
					m_playerState = player_state_stop;
					return false;
				}
			}

			bool mediaSetPlay()
			{
				if (m_file.isValid())
				{
					m_audioPlayer.play();
					m_playerState = player_state_playing;
					craft_engine_gui_emit(mediaPlay, );
					return true;
				}
				else
				{
					m_playerState = player_state_stop;
					return false;
				}
			}

			bool mediaIsValid()const
			{
				return m_file.isValid();
			}

		protected:

			virtual void onPaintEvent() override
			{
				process();
				auto painter = getPainter();
				painter.drawRect(Rect(0, 0, getWidth(), getHeight()), Color(0, 0, 0, 255));


				if (m_frameImage != nullptr)
				{
					int width = getWidth();
					int heigth = getHeight();
					if (float(width) / float(heigth) > float(m_file.getVideoWidth()) / float(m_file.getVideoHeight()))
					{
						width = heigth * m_file.getVideoWidth() / m_file.getVideoHeight();
					}
					else
					{
						heigth = width * m_file.getVideoHeight() / m_file.getVideoWidth();
					}

					HandleImage output = m_frameImage;

					painter.drawRect(
						Rect((getWidth() - width) / 2, (getHeight() - heigth) / 2, width, heigth),
						output);
				}

				drawAllChildren();
			}


		};



	}
}

#endif // CRAFT_ENGINE_GUI_MEDIA_PLAYER_WIDGET_H_