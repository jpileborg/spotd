/* -*- coding: utf-8 -*- */
/* *******************************************************************
* File: test-spot.cpp                                  Part of spotd *
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

// This program is my attempt to learn the libspotify API, and how to
// use it to search for and play music.

// BIG IMPORTANT NOTE: This code needs a compiler capable of the new
// C++11 standard! It also needs a standard library updated most of
// the latest stuff from the C++11 standard.

// Song to play: Doctor! Doctor!, by Thompson Twins
//     (spotify:track:0P412IP7uGemFJNx3ccPAB)
//
// Album to play: Dopes to Infinity, by Monster Magnet
//
// Playlist to play: Pilens 90-tal
//
// Artist to search for: Bob Hund
//
// Also do general searching, and this is where I should start.

/* **************************************************************** */

#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <exception>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <signal.h>

#include <libspotify/api.h>

#include "alsa.h"

/* **************************************************************** */

namespace
{
    bool keep_running = false;
}

/* **************************************************************** */

namespace appkey
{
    const uint8_t key[] = {
        0x01, 0xA7, 0x42, 0xD8, 0xD3, 0x73, 0x4D, 0x60, 0x88, 0x7E, 0x3D, 0x47, 0x6F, 0x3B, 0xFD, 0xD3,
        0x71, 0xDA, 0x4B, 0xF8, 0x0C, 0x9A, 0x3C, 0x67, 0x21, 0x74, 0xA5, 0xFB, 0x22, 0x8E, 0xA9, 0x98,
        0x6C, 0x4F, 0x97, 0x7D, 0xD3, 0xBD, 0x97, 0x80, 0xA3, 0xDD, 0x6E, 0x0C, 0x9E, 0x61, 0x9B, 0xB7,
        0x8F, 0xAF, 0xE2, 0xFB, 0xF1, 0x0F, 0x29, 0x52, 0x22, 0x75, 0xEB, 0x96, 0xA9, 0x43, 0x7D, 0x04,
        0x1E, 0x82, 0x03, 0x31, 0xCD, 0x3B, 0x2E, 0x46, 0x6F, 0x4F, 0xCC, 0x5D, 0x79, 0xAC, 0x55, 0xB9,
        0xEA, 0x68, 0x3F, 0xA6, 0xE4, 0x4C, 0xC1, 0xF3, 0xE9, 0x4D, 0x98, 0xEB, 0x37, 0xA7, 0x7B, 0x18,
        0x7C, 0x2F, 0xAA, 0xD8, 0x4F, 0xC5, 0xF5, 0x28, 0xA6, 0x9E, 0xEE, 0x62, 0x55, 0x07, 0x88, 0xF6,
        0x76, 0x9A, 0x39, 0xE6, 0x34, 0x29, 0x5C, 0x56, 0x38, 0x26, 0x08, 0x08, 0xE3, 0xF0, 0x92, 0xE4,
        0x69, 0x84, 0x7E, 0x26, 0x44, 0xA8, 0x19, 0xEA, 0x77, 0xAF, 0x31, 0xDB, 0xFD, 0x08, 0x01, 0xB1,
        0x88, 0x5B, 0x10, 0xF9, 0x00, 0x07, 0x94, 0xB3, 0xEA, 0xCB, 0xB8, 0x2D, 0x35, 0x84, 0x40, 0xB8,
        0xFC, 0xE2, 0x2C, 0x7D, 0x33, 0x88, 0x38, 0x1B, 0x2C, 0x2B, 0xF2, 0x5E, 0xC0, 0x72, 0x09, 0x78,
        0x8F, 0x6D, 0x03, 0xD5, 0x52, 0x76, 0xC1, 0xC5, 0x7C, 0xAA, 0xC3, 0xF2, 0x27, 0x30, 0x46, 0x14,
        0x04, 0xA7, 0x73, 0x6B, 0x08, 0x5E, 0x95, 0x13, 0x5A, 0xF1, 0x7B, 0xD9, 0xB0, 0x5F, 0xB3, 0x1C,
        0x33, 0xC3, 0xF1, 0x09, 0x97, 0x9C, 0xF9, 0x70, 0x8D, 0x29, 0x03, 0xE9, 0xB1, 0x39, 0x62, 0x87,
        0xF3, 0xDD, 0x11, 0x7D, 0x71, 0x3D, 0xF6, 0xA4, 0x76, 0xF5, 0xF7, 0x4E, 0xF2, 0xD0, 0x38, 0x49,
        0x6F, 0xAA, 0x3D, 0xF5, 0x95, 0x04, 0xF9, 0x48, 0x67, 0x2F, 0x69, 0x8D, 0x8D, 0xBB, 0xDD, 0xFB,
        0xF8, 0x74, 0x59, 0x85, 0x99, 0x1A, 0x70, 0x68, 0xE4, 0x37, 0x57, 0xC1, 0x84, 0x33, 0x2E, 0x89,
        0xD9, 0x7E, 0xD7, 0x48, 0x9C, 0x77, 0x2F, 0x2A, 0x36, 0xE3, 0xCC, 0x34, 0x85, 0x53, 0xE2, 0x7A,
        0x19, 0x8C, 0x29, 0xD0, 0xF1, 0x96, 0xFE, 0xE7, 0x6F, 0xDE, 0xD9, 0x6C, 0xC2, 0xDD, 0x00, 0x08,
        0x37, 0xDC, 0x66, 0x8C, 0x10, 0x8A, 0x46, 0x12, 0xC0, 0x45, 0xC8, 0x31, 0xAD, 0xAA, 0xD4, 0xAD,
        0x89,
    };

    const size_t size = sizeof(key);
}

/* **************************************************************** */

namespace session
{
    sp_session *session = nullptr;

    std::mutex notify_mutex;
    std::unique_lock<std::mutex> notify_lock(notify_mutex);
    std::condition_variable notify_cond;
    bool notify_events = false;
    bool is_logged_in = false;
    bool player_is_configured = false;

    /* ************************************************************ */

    namespace callbacks
    {
        void logged_in(sp_session *session, sp_error error)
        {
            std::cout << "logged in\n";

            if (error != SP_ERROR_OK)
            {
                std::cout << "Error logging in: " << sp_error_message(error) << "\n";
                sp_session_release(session);
                exit(10);
            }

            sp_user *me = sp_session_user(session);

            if (sp_user_is_loaded(me))
                std::cout << "    display name: " << sp_user_display_name(me) << "\n";
            else
                std::cout << "    canonical name: " << sp_user_canonical_name(me) << "\n";

            int cc = sp_session_user_country(session);
            std::cout << "    country: " << char(cc >> 8) << char(cc & 0x0ff)
                      << " (" << std::setw(2) << std::setfill('0') << std::hex << cc << std::dec << ")\n";

            is_logged_in = true;
        }

        void logged_out(sp_session *session)
        {
            is_logged_in = false;
            std::cout << "logged out\n";
            keep_running = false;
        }



        void connection_error(sp_session *session, sp_error error)
        {
            std::cout << "connection error: " << sp_error_message(error) << "\n";
        }


        void message_to_user(sp_session *session, const char *message)
        {
            std::cout << "message: " << message << std::endl;
        }

        void notify_main_thread(sp_session *session)
        {
            notify_mutex.lock();
            notify_events = true;
            notify_cond.notify_all();
            notify_mutex.unlock();
        }

        int music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
        {
            if (!player_is_configured)
            {
                std::cout << "setting audio parameters:\n";
                std::cout << "    sample type: " << format->sample_type << "\n";
                std::cout << "    sample rate: " << format->sample_rate << "\n";
                std::cout << "    channels   : " << format->channels << "\n";

                if (alsa::set_parameters(format->sample_type, format->sample_rate, format->channels))
                    player_is_configured = true;
            }

            if (alsa::queue_samples(frames, num_frames))
                return num_frames;
            else
                return 0;
        }



        void log_message(sp_session *session, const char *data)
        {
            std::cout << "log: " << data;
        }



        void userinfo_updated(sp_session *session)
        {
            std::cout << "userinfo updated\n";

            // XXX What is the meaning of this callback?
            // XXX Is it the user-info of the logged in user thats been updated
            // XXX (like I assume it is at the moment)?

            sp_user *me = sp_session_user(session);

            if (sp_user_is_loaded(me))
                std::cout << "    display name: " << sp_user_display_name(me) << "\n";
            else
                std::cout << "    canonical name: " << sp_user_canonical_name(me) << "\n";

            int cc = sp_session_user_country(session);
            std::cout << "    country: " << char(cc >> 8) << char(cc & 0x0ff)
                      << " (" << std::setw(2) << std::setfill('0') << std::hex << cc << std::dec << ")\n";
        }



        void offline_status_updated(sp_session *session)
        {
            std::cout << "offline status updated\n";
        }

        // TODO: More callbacks!

        const sp_session_callbacks callbacks = {
            logged_in,              // logged_in
            logged_out,             // logged_out
            nullptr,                // metadata_updated 
            connection_error,       // connection_error
            message_to_user,        // message_to_user
            notify_main_thread,     // notify_main_thread
            music_delivery,         // music_delivery
            nullptr,                // play_token_lost
            log_message,            // log_message
            nullptr,                // end_of_track
            nullptr,                // streaming_error
            userinfo_updated,       // userinfo_updated
            nullptr,                // start_playback
            nullptr,                // stop_playback
            nullptr,                // get_audio_buffer_stats
            offline_status_updated, // offline_status_updated
        };
    }

    /* ************************************************************ */

    bool create(const char *username, const char *password)
    {
        sp_session_config cfg;
        sp_session *s;

        std::cout << "session::create()\n";

        std::cout << "    configuration\n";
        std::memset(&cfg, 0, sizeof(cfg));
        cfg.api_version = SPOTIFY_API_VERSION;
        cfg.cache_location = "tmp";
        cfg.settings_location = "tmp";
        cfg.application_key = appkey::key;
        cfg.application_key_size = appkey::size;
        cfg.user_agent = "spotd-test";
        cfg.callbacks = &callbacks::callbacks;

        std::cout << "    appkey::size = " << appkey::size << "\n";

        std::cout << "    creating session\n";
        sp_error error = sp_session_create(&cfg, &s);
        if (error != SP_ERROR_OK)
        {
            std::cout << "Error creating session: "
                      << sp_error_message(error) << "\n";
            return false;
        }
        std::cout << "        session = " << std::setw(8) << std::setfill('0') << std::hex << s << std::dec << "\n";

        std::cout << "    logging in\n";
        std::cout << "        username = \"" << username << "\"\n";
        std::cout << "        password = \"" << password << "\"\n";

        sp_session_login(s, username, password, 0);
        session = s;

        std::cout << "    done\n";
        return true;
    }

    void destroy()
    {
        if (session)
        {
            sp_session_logout(session);
            sp_session_release(session);
            session = nullptr;
        }
    }
}

/* **************************************************************** */

namespace search
{
    namespace
    {
        void print_track(sp_track *track)
        {
            int duration = sp_track_duration(track);

            std::cout << "Track \"" << sp_track_name(track) << "\"\n";

            std::cout << "    Starred    : "
                      << (sp_track_is_starred(session::session, track) ? "yes" : "no") << "\n";
            std::cout << "    Duration   : " << (duration / 60000) << ":"
                      << std::setfill('0') << std::setw(2) << ((duration / 1000) / 60) << "\n";
            std::cout << "    Artists    : " << sp_track_num_artists(track) << "\n";
            std::cout << "    Popularity : " << sp_track_popularity(track) << "\n";

            sp_album *album = sp_track_album(track);
            if (album != nullptr)
                std::cout << "    Album      : " << sp_album_name(album) << "\n";

            sp_link *link = sp_link_create_from_track(track, 0);
            char url[256];
            sp_link_as_string(link, url, sizeof(url) - 1);
            sp_link_release(link);
            std::cout << "    Spotify URL: " << url << "\n";

            std::cout << "    Artists    :\n";
            std::cout << std::setfill(' ');
            for (int i = 0; i < sp_track_num_artists(track); i++)
            {
                sp_artist *artist = sp_track_artist(track, i);

                std::cout << "        " << std::setw(2) << (i + 1) << ": "
                          << sp_artist_name(artist) << "\n";
            }
        }

        void print_artist(sp_artist *artist)
        {
        }

        void print_album(sp_album *album)
        {
            std::cout << "Album \"" << sp_album_name(album) << "\"\n";
            std::cout << "    Year     : " << sp_album_year(album) << "\n";
            std::cout << "    Available: " << (sp_album_is_available(album) ? "yes" : "no") << "\n";
            // std::cout << "    Cover ID : " << sp_album_cover(album) << "\n";

            sp_artist *artist = sp_album_artist(album);
            std::cout << "    Artist   : " << sp_artist_name(artist) << "\n";

            const char *album_types[] = {
                "Album",
                "Single",
                "Compilation",
                "Unknown"
            };

            std::cout << "    Type     : " << album_types[sp_album_type(album)] << "\n";
        }

        void search_complete(sp_search *search, void *)
        {
            if (sp_search_error(search) != SP_ERROR_OK)
            {
                std::cout << "search error: "
                          << sp_error_message(sp_search_error(search)) << "\n";
            }
            else
            {
                std::cout << "\n";
                std::cout << "Search results\n--------------\n";
                std::cout << "Query       : " << sp_search_query(search) << "\n";
                std::cout << "Did you mean: " << sp_search_did_you_mean(search) << "\n";
                std::cout << "Tracks      : " << sp_search_num_tracks(search)
                          << "/" << sp_search_total_tracks(search) << "\n";
                std::cout << "Artists     : " << sp_search_num_artists(search)
                          << "/" << sp_search_total_artists(search) << "\n";
                std::cout << "Albums      : " << sp_search_num_albums(search)
                          << "/" << sp_search_total_albums(search) << "\n";
                std::cout << "\n";

                for (int i = 0; i < sp_search_num_tracks(search); i++)
                    print_track(sp_search_track(search, i));

                for (int i = 0; i < sp_search_num_artists(search); i++)
                    print_artist(sp_search_artist(search, i));

                for (int i = 0; i < sp_search_num_albums(search); i++)
                    print_album(sp_search_album(search, i));
            }

            sp_search_release(search);
        }
    }

    /* ************************************************************ */

    void search_track(const char *query)
    {
        sp_search_create(session::session, query, 0, 5, 0, 0, 0, 0, search_complete, nullptr);
    }

    void search_album(const char *query)
    {
        sp_search_create(session::session, query, 0, 0, 0, 5, 0, 0, search_complete, nullptr);
    }


    /* ************************************************************ */

    namespace
    {
        bool search_track_done = false;
        bool search_album_done = false;
        bool search_artist_done = false;
        bool search_playlist_done = false;
    }

    void test_search()
    {
        if (!search_track_done)
        {
            search_track_done = true;
            search_track("track:\"Doctor! Doctor!\" artist:\"Thompson Twins\" album:\"Into The Gap\"");
            return;
        }

        if (!search_album_done)
        {
            search_album_done = true;
            search_album("album:\"Dopes to Infinity\" artist:\"Monster Magnet\"");
            return;
        }

        if (!search_artist_done)
        {
            search_artist_done = true;
            return;
        }

        if (!search_playlist_done)
        {
            search_playlist_done = true;
            return;
        }
    }
}

/* **************************************************************** */

/* **************************************************************** */

namespace posix
{
    void handle_sig_hup(int)
    {
        keep_running = false;
    }

    /* ************************************************************ */

    bool init()
    {
        signal(SIGHUP , handle_sig_hup);
        signal(SIGINT , handle_sig_hup);
        signal(SIGTERM, handle_sig_hup);
        signal(SIGQUIT, handle_sig_hup);

        return true;
    }

    void clean()
    {
    }
}

/* **************************************************************** */

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <username> <password>\n";
        return 1;
    }

    std::set_terminate(__gnu_cxx::__verbose_terminate_handler);

    std::cout << "main: unlock notification mutex\n";
    session::notify_mutex.unlock();

    if (!alsa::open())
        return 2;

    if (!session::create(argv[1], argv[2]))
    {
        alsa::close();
        return 3;
    }

    if (!posix::init())
    {
        session::destroy();
        alsa::close();
        return 4;
    }

    keep_running = true;
    int next_timeout = 0;

    std::cout << "main: lock notification mutex\n";
    session::notify_mutex.lock();

    std::cout << "entering main loop\n";
    while (keep_running)
    {
        if (next_timeout == 0)
        {
            while (keep_running && !session::notify_events)
                session::notify_cond.wait(session::notify_lock);
        }
        else
        {
            while (keep_running && !session::notify_events)
            {
                std::cv_status status = session::notify_cond.wait_for(
                    session::notify_lock, std::chrono::milliseconds(next_timeout));
                if (status == std::cv_status::no_timeout)
                {
                    break;
                }
            }
        }

        if (session::is_logged_in)
            search::test_search();

        session::notify_events = false;
        session::notify_mutex.unlock();

        do
        {
            sp_session_process_events(session::session, &next_timeout);
        } while (keep_running && next_timeout == 0);

        session::notify_mutex.lock();
    }

    std::cout << "\nmain loop done, cleaning up\n";

    std::cout << "main: unlock notification mutex\n";
    session::notify_mutex.unlock();

    posix::clean();
    session::destroy();
    alsa::close();

    std::cout << "all done, exiting\n";
}

/* **************************************************************** */
