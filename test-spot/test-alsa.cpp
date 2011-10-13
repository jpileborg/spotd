/* -*- coding: utf-8 -*- */
/* *******************************************************************
* File: test-alsa.cpp                                  Part of spotd *
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

#define _GLIBCXX_USE_NANOSLEEP

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <exception>
#include <cmath>
#include <signal.h>

#include <libspotify/api.h>

#include "alsa.h"

namespace this_thread = std::this_thread;

/* **************************************************************** */

namespace
{
    bool keep_running = true;
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

int main()
{
    std::set_terminate(__gnu_cxx::__verbose_terminate_handler);

    std::cout << "_POSIX_C_SOURCE = " << _POSIX_C_SOURCE << "\n";

    signed short data[8000];

    std::cout << "calling posix::init()\n";
    if (!posix::init())
    {
        std::cout << "posix::init() failed\n";
        return 1;
    }

    std::cout << "calling alsa::open()\n";
    if (!alsa::open())
    {
        std::cout << "alsa::open() failed\n";
        posix::clean();
        return 2;
    }

    std::cout << "creating data\n";
    double step = (2 * M_PI) / (4000 / 1000);
    double rad  = 0;
    for (int i = 0; i < 8000; i++)
    {
        // data[i] = static_cast<signed short>(std::sin(rad) * 32767);
        data[i] = static_cast<signed short>(std::sin(rad) * 5000);
        rad += step;
    }

    std::cout << "calling alsa::set_parameters()\n";
    if (!alsa::set_parameters(SP_SAMPLETYPE_INT16_NATIVE_ENDIAN, 4000, 1))
    {
        std::cout << "alsa::set_parameters() failed\n";
        alsa::close();
        posix::clean();
        return 3;
    }

    std::cout << "entering main loop\n";
    while (keep_running)
    {
        alsa::queue_samples(data, 8000);
        this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    std::cout << "main loop done\n";

    alsa::close();
    posix::clean();
}

/* **************************************************************** */
