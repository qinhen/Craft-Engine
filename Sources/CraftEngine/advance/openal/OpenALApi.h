#pragma once
#ifndef CRAFT_ENGINE_OPENAL_API_H_
#define CRAFT_ENGINE_OPENAL_API_H_


#include "../../3rdparty/ThirdPartyImportOpenAL.h"

#include <iostream>
#include <assert.h>
#include <queue>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <chrono>

namespace CraftEngine
{
    namespace openal
    {

        bool error_detect()
        {
            auto err = alGetError();
            if (err != AL_NO_ERROR)
            {
                std::cout << "error in openal call. code:" << err << std::endl;
                return false;
            }
            else
                return true;
        }

        struct Buffer
        {
            ALuint m_buffer = 0;

            Buffer()
            {
                static_assert(sizeof(Buffer) == sizeof(ALuint), "sizeof(Buffer) != sizeof(ALuint)");
            }

            bool isValid() { return alIsBuffer(m_buffer); }

            bool setPcmDataFloat32(float* data, uint32_t sample_count, uint32_t sample_rate)
            {
                /* Buffer the audio data into a new buffer object. */
                uint32_t data_size = sample_count * 2 * sizeof(float);
                uint32_t freq = sample_rate;

                //std::cout << "data_size = " << data_size << std::endl;

                alBufferData(m_buffer, AL_FORMAT_STEREO_FLOAT32, data, (ALsizei)data_size, (ALsizei)freq);
                error_detect();
                /* Check if an error occured, and clean up if so. */
                return error_detect();
            }
            bool setDataInt16(short* data, uint32_t sample_count, uint32_t sample_rate)
            {
                /* Buffer the audio data into a new buffer object. */
                uint32_t data_size = sample_count * 2 * sizeof(short);
                uint32_t freq = sample_rate;
                alBufferData(m_buffer, AL_FORMAT_STEREO16, data, (ALsizei)data_size, (ALsizei)freq);
                /* Check if an error occured, and clean up if so. */
                return error_detect();
            }

         

        };



        struct Source
        {

            ALuint m_source = 0;
  
            Source()
            {
                static_assert(sizeof(Source) == sizeof(ALuint), "sizeof(Source) != sizeof(ALuint)");
            }

            bool isValid() { if (m_source == 0) return false; else return alIsSource(m_source); }

            bool testState(ALint state)
            {
                ALint s = 0;
                alGetSourcei(m_source, AL_SOURCE_STATE, &s);
                if (s == state)
                    return true;
                else
                    return false;
            }

            int32_t getInt(ALint type)
            {
                ALint val = 0;
                alGetSourcei(m_source, type, &val);
                return val;
            }


            bool isPlaying() { return testState(AL_PLAYING); }
            bool isInitial() { return testState(AL_INITIAL); }
            bool isPaused() { return testState(AL_PAUSED); }
            bool isStopped() { return testState(AL_STOPPED); }

            void play() { alSourcePlay(m_source); }
            void stop() { alSourceStop(m_source); }
            void pause() { alSourcePause(m_source); }


            void setBuffer(const Buffer& buffer = Buffer())
            {
                alSourcei(m_source, AL_BUFFER, buffer.m_buffer);
                error_detect();
            }

            void enqueueBuffers(uint32_t count, const Buffer* buffers)
            {
                alSourceQueueBuffers(m_source, count, (ALuint*)buffers);
                error_detect();
            }

            void dequeueBuffers(uint32_t count, const Buffer* buffers)
            {
                alSourceUnqueueBuffers(m_source, count, (ALuint*)buffers);
                error_detect();
            }

            uint32_t getQueueProcessedBuffersCount() { return getInt(AL_BUFFERS_PROCESSED); }
            uint32_t getQueueQueuedBuffersCount() { return getInt(AL_BUFFERS_QUEUED); }
            uint32_t getOffset() { return getInt(AL_SAMPLE_OFFSET); }
        };


  
        static Buffer createBuffer()
        {
            Buffer buffer;
            alGenBuffers(1, &buffer.m_buffer);
            auto err = alGetError();
            if (err != AL_NO_ERROR)
            {
                throw 0;
                std::cout << "error in createBuffer call. code:" << err << std::endl;
            }
            return buffer;
        }

        void deleteBuffer(Buffer buf)
        {
            alDeleteBuffers(1, &buf.m_buffer);
            error_detect();
        }
        void deleteBuffers(uint32_t count, Buffer* bufs)
        {
            alDeleteBuffers(count, &bufs[0].m_buffer);
            error_detect();
        }


        static Source createSource()
        {
            Source source;
            alGenSources(1, &source.m_source);
            auto err = alGetError();
            error_detect();
            return source;
        }

        void deleteSource(Source src)
        {
            alDeleteBuffers(1, &src.m_source);
            error_detect();
        }





        class AudioStreamPlayer
        {
        public:

            std::queue<Buffer> m_buffers;
            Source m_source;
            std::mutex m_mutex;

            void addStreamBuffer(uint32_t count, Buffer* bufs)
            {
                m_source.enqueueBuffers(count, bufs);
                for (int i = 0; i < count; i++)
                    m_buffers.push(bufs[i]);
            }
            uint32_t getCurQueuedBuffer()
            {
                return m_source.getQueueQueuedBuffersCount() - m_source.getQueueProcessedBuffersCount();
            }
            uint32_t getCurUsedBuffer()
            {
                return m_source.getQueueProcessedBuffersCount();
            }

            enum PlayState
            {
                play_state_initial,
                play_state_playing,
                play_state_pause,
                play_state_stop,
            };


            PlayState m_state;

        public:

            AudioStreamPlayer()
            {
                m_state = play_state_initial;
            }

            ~AudioStreamPlayer()
            {
                m_state = play_state_initial;
                if (m_source.isValid())
                {
                    deleteSource(m_source);
                }
            }

            void init()
            {
                m_source = createSource();
            }

            bool isPlaying() { return m_source.isPlaying(); }

            bool play()
            {
                m_state = play_state_playing;
                m_source.play();
                return true;
            }

            void pause()
            {
                m_state = play_state_pause;
                m_source.pause();
            }

            void stop_lock()
            {
                {
                    std::lock_guard<std::mutex> mutex(m_mutex);
                    m_state = play_state_stop;
                    m_source.stop();
                }
                removeAllStreamBuffer();
            }



            int removeInvalidStreamBuffer()
            {
                std::lock_guard<std::mutex> mutex(m_mutex);
                auto dequeue_bf_count = m_source.getQueueProcessedBuffersCount();
                dequeue_bf_count = dequeue_bf_count < m_buffers.size() ? dequeue_bf_count : m_buffers.size();
                if (dequeue_bf_count == 0)
                    return 0;

                std::vector<Buffer> dequeue_bufs(dequeue_bf_count);
                for (int i = 0; i < dequeue_bf_count; i++)
                {
                    dequeue_bufs[i] = m_buffers.front();
                    m_buffers.pop();
                }
                if (dequeue_bf_count > 0)
                {
                    m_source.dequeueBuffers(dequeue_bufs.size(), dequeue_bufs.data());
                    deleteBuffers(dequeue_bufs.size(), dequeue_bufs.data());
                }
                return dequeue_bf_count;
            }


            int removeAllStreamBuffer()
            {
                std::lock_guard<std::mutex> mutex(m_mutex);
                auto dequeue_bf_count = m_buffers.size();
                std::vector<Buffer> dequeue_bufs(dequeue_bf_count);
                for (int i = 0; i < dequeue_bf_count; i++)
                {
                    dequeue_bufs[i] = m_buffers.front();
                    m_buffers.pop();
                }
                if (dequeue_bufs.size() > 0)
                {
                    m_source.dequeueBuffers(dequeue_bufs.size(), dequeue_bufs.data());
                    deleteBuffers(dequeue_bufs.size(), dequeue_bufs.data());
                }
                return dequeue_bf_count;
            }


            void enqueue_lock(std::vector<Buffer> buffers)
            {
                addStreamBuffer(buffers.size(), buffers.data());
                if (m_state == play_state_playing && !m_source.isPlaying())
                {
                    m_source.play();
                }
            }


            int getOffset()
            {
                return m_source.getOffset();
            }

        };





        class OpenALSystem
        {
        private:

            static bool m_isInitialized;

        public:

            /* InitAL opens a device and sets up a context using default attributes, making
             * the program ready to call OpenAL functions. */
            static bool init(char* device_name = nullptr)
            {
                if (!m_isInitialized)
                {
                    m_isInitialized = false;
                    const ALCchar* name;
                    ALCdevice* device;
                    ALCcontext* ctx;

                    /* Open and initialize a device */
                    device = NULL;
                    if (device_name != nullptr)
                    {
                        device = alcOpenDevice(device_name);
                        if (!device)
                            fprintf(stderr, "Failed to open \"%s\", trying default\n", device_name);
                    }
                    else
                    {
                        if (!device)
                            device = alcOpenDevice(NULL);
                    }

                    if (!device)
                    {
                        fprintf(stderr, "Could not open a device!\n");
                        return false;
                    }

                    ctx = alcCreateContext(device, NULL);
                    if (ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE)
                    {
                        if (ctx != NULL)
                            alcDestroyContext(ctx);
                        alcCloseDevice(device);
                        fprintf(stderr, "Could not set a context!\n");
                        return 1;
                    }

                    name = NULL;


                    if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
                        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
                    //printf("ALC_ALL_DEVICES_SPECIFIER \"%s\"\n", name);
                    if (!name || alcGetError(device) != AL_NO_ERROR)
                        name = alcGetString(device, ALC_DEVICE_SPECIFIER);
                    printf("Opened \"%s\"\n", name);
                    return true;
                }         
                return false;
            }

            /* CloseAL closes the device belonging to the current context, and destroys the
             * context. */
            static void close(void)
            {
                if (m_isInitialized)
                {
                    m_isInitialized = false;
                    ALCdevice* device;
                    ALCcontext* ctx;
                    ctx = alcGetCurrentContext();
                    if (ctx == NULL)
                        return;
                    device = alcGetContextsDevice(ctx);
                    alcMakeContextCurrent(NULL);
                    alcDestroyContext(ctx);
                    alcCloseDevice(device);
                }
            }

            static void wait(Source& source, uint32_t timeout_ms, uint32_t duration_ms = 20)
            {
                uint32_t time_acc = 0;
                while (1)
                {
                    if (time_acc >= timeout_ms)
                        break;
                    if(!source.isPlaying())
                        break;
                    time_acc += duration_ms;
                    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
                }
            }



        };

        bool OpenALSystem::m_isInitialized = false;

    }



}

#endif // !CRAFT_ENGINE_OPENAL_API_H_