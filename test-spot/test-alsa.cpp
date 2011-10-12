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

#include <iostream>
#include <iomanip>
#include <signal.h>

#include <libspotify/api.h>

#include "alsa.h"

/* **************************************************************** */

namespace
{
    bool keep_running = false;
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
    signed short *data;

    if (!posix::init())
        return 1;

    if (!alsa::open())
    {
        posix::clean();
        return 2;
    }

    // TODO: Create buffer with sine-wave audio data

    if (!alsa::set_parameters(SP_SAMPLETYPE_INT16_NATIVE_ENDIAN, 4000, 1))
    {
        alsa::close();
        posix::clean();
        return 3;
    }

    while (keep_running)
    {
        // TODO: Do stuff!
        // bool queue_samples(const void *frames, const int samples)
    }

    alsa::close();
    posix::clean();
}

/* **************************************************************** */
