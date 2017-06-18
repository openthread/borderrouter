/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

#include "otbr-config.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "border_agent.hpp"
#include "common/code_utils.hpp"
#include "common/logging.hpp"

static const char kSyslogIdent[] = "otbr-agent";
static const char kDefaultInterfaceName[] = "wpan0";

// Default poll timeout.
static const struct timeval kPollTimeout = {10, 0};

int Mainloop(const char *aInterfaceName)
{
    int rval = 0;

    ot::BorderRouter::BorderAgent br(aInterfaceName);

    while (true)
    {
        fd_set         readFdSet;
        fd_set         writeFdSet;
        fd_set         errorFdSet;
        int            maxFd = -1;
        struct timeval timeout = kPollTimeout;

        FD_ZERO(&readFdSet);
        FD_ZERO(&writeFdSet);
        FD_ZERO(&errorFdSet);

        br.UpdateFdSet(readFdSet, writeFdSet, errorFdSet, maxFd, timeout);
        rval = select(maxFd + 1, &readFdSet, &writeFdSet, &errorFdSet, &timeout);

        if ((rval < 0) && (errno != EINTR))
        {
            rval = errno;
            perror("select failed");
            break;
        }

        br.Process(readFdSet, writeFdSet, errorFdSet);
    }

    return rval;
}

void PrintVersion(void)
{
    printf("%s\n", PACKAGE_VERSION);
}

int main(int argc, char *argv[])
{
    const char *interfaceName = kDefaultInterfaceName;
    int         logLevel = OTBR_LOG_INFO;
    int         opt;
    int         ret = 0;

    while ((opt = getopt(argc, argv, "d:I:v")) != -1)
    {
        switch (opt)
        {
        case 'd':
            logLevel = atoi(optarg);
            break;

        case 'I':
            interfaceName = optarg;
            break;

        case 'v':
            PrintVersion();
            ExitNow();
            break;

        default:
            fprintf(stderr, "Usage: %s [-I interfaceName] [-d DEBUG_LEVEL] [-v]\n", argv[0]);
            ExitNow(ret = -1);
            break;
        }
    }

    otbrLogInit(kSyslogIdent, logLevel);
    otbrLog(OTBR_LOG_INFO, "Border router agent started on %s", interfaceName);

    ret = Mainloop(interfaceName);

    otbrLogDeinit();

exit:
    return ret;
}
