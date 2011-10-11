/* -*- coding: utf-8 -*- */
/* *******************************************************************
* File: test-spot/alsa.cpp                             Part of spotd *
*                                                                    *
* Copyright (C) 2011, Joachim Pileborg and individual contributors.  *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or without *
* modification, are permitted provided that the following conditions *
* are met:                                                           *
*                                                                    *
*   o Redistributions of source code must retain the above copyright *
*     notice, this list of conditions and the following disclaimer.  *
*   o Redistributions in binary form must reproduce the above        *
*     copyright notice, this list of conditions and the following    *
*     disclaimer in the documentation and/or other materials         *
*     provided with the distribution.                                *
*   o Neither the name of The Raven nor the names of its contributors*
*     may be used to endorse or promote products derived from this   *
*     software without specific prior written permission.            *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS  *
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,*
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,           *
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR *
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       *
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF *
* THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF    *
* SUCH DAMAGE.                                                       *
*                                                                    *
******************************************************************* */

// The purpose of this file is to play the audio PCM data received from
// libspotify. This is done with the help of ALSA.

#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <cstring>

#include <asoundlib.h>
#include <libspotify/api.h>

/* **************************************************************** */

namespace alsa
{
    namespace
    {
        snd_pcm_t *handle = 0;

        struct sample_data
        {
            int   samples;     // number of samples in the data
            char  frames[0];   // the PCM data
        };

        std::queue<sample_data *> sample_queue;

        std::mutex sample_queue_mutex;
        std::unique_lock<std::mutex> sample_queue_lock(sample_queue_mutex);

        std::thread player_thread;

        bool kill_player_thread = false;

        /* ******************************************************** */

        void player()
        {
            while (!kill_player_thread)
            {
                if (!sample_queue.empty())
                {
                    sample_data *s;

                    if (sample_queue_lock.try_lock())
                    {
                        s = sample_queue.front();
                        sample_queue.pop();
                        sample_queue_lock.unlock();
                    }

                    // Play the sample data
                    // s->samples * 2 because one sample is two bytes
                    int frames = snd_pcm_writei(handle, s->frames, s->samples * 2);

                    if (frames < 0)
                        frames = snd_pcm_recover(handle, frames, 0);
                    if (frames < 0)
                        std::cout << "Error playing sample: " << snd_strerror(frames) << "\n";

                    if (frames > 0 && frames < (s->samples * 2))
                    {
                        std::cout << "Error playing sample: short write\n";
                        std::cout << "    Expected " << (s->samples * 2)
                                  << " bytes, wrote " << frames << " bytes\n";
                    }
                }
            }
        }
    }

    bool open(const char *device /* = "default" */)
    {
        // int error = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);
        int error = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
        if (error < 0)
        {
            std::cout << "Error opening ALSA device: " << snd_strerror(error) << "\n";
            return false;
        }

        player_thread = std::thread(player);

        sample_queue_lock.unlock();

        return true;
    }

    void close()
    {
        kill_player_thread = true;
        player_thread.join();

        if (handle != 0)
        {
            snd_pcm_close(handle);
            handle = 0;
        }
    }

    bool set_parameters(const sp_sampletype sample_type, const int sample_rate, const int channels)
    {
        snd_pcm_format_t format;

        switch (sample_type)
        {
        case SP_SAMPLETYPE_INT16_NATIVE_ENDIAN:
            format = SND_PCM_FORMAT_S16;
            break;

        default:
            std::cout << "Error setting parameters: unknown sample type " << sample_type << "\n";
            return false;
        }

        int error = snd_pcm_set_params(handle, format,
                                       SND_PCM_ACCESS_RW_INTERLEAVED,
                                       channels,
                                       sample_rate,
                                       1,
                                       500000);   // 0.5sec overal latency
        if (error < 0)
        {
            std::cout << "Error setting parameters: " << snd_strerror(error) << "\n";
            return false;
        }

        return true;
    }

    /* ************************************************************ */

    bool queue_samples(const void *frames, const int samples)
    {
        // samples * 2 because each sample is 2 bytes (16 bits)
        sample_data *s = reinterpret_cast<sample_data *>(new char [sizeof(struct sample_data) + (samples * 2)]);
        std::memcpy(s->frames, frames, samples * 2);
        s->samples = samples;

        if (!sample_queue_lock.try_lock())
            return false;

        sample_queue.push(s);
        sample_queue_lock.unlock();

        return true;
    }

    /* ************************************************************ */

    /* ************************************************************ */
}

/* **************************************************************** */
