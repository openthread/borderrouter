/*
 *    Copyright (c) 2020, The OpenThread Authors.
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

/**
 * @file
 *   This file includes definition for Thread backbone agent.
 */

#ifndef BACKBONE_AGENT_HPP_
#define BACKBONE_AGENT_HPP_

#include <openthread/backbone_router_ftd.h>

#include "agent/ncp_openthread.hpp"
#include "backbone/smcroute_manager.hpp"

namespace otbr {

namespace Backbone {

/**
 * @addtogroup border-router-backbone-agent
 *
 * @brief
 *   This module includes definition for Thread backbone agent
 *
 * @{
 */

/**
 * This class implements Thread backbone agent functionality.
 *
 */
class BackboneAgent
{
public:
    BackboneAgent(otbr::Ncp::ControllerOpenThread *aThread);
    void Init(const std::string &aThreadIfName, const std::string &aBackboneIfName);

    void HandleBackboneRouterState(void);
    void HandleBackboneRouterLocal(void);

    void HandleBackboneRouterMulticastListenerEvent(otBackboneRouterMulticastListenerEvent aEvent,
                                                    const otIp6Address &                   aAddress);

private:
    otbr::Ncp::ControllerOpenThread &mThread;
    otBackboneRouterState            mBackboneRouterState;
    SmcrouteManager                  mSmcrouteManager;

    void Log(int aLevel, const char *aFormat, ...);
    void BackboneUp(void);
    void BackboneDown(void);
    void EnterPrimary(void);
    void ExitPrimary(void);
    bool IsPrimary(void) { return mBackboneRouterState == OT_BACKBONE_ROUTER_STATE_PRIMARY; }
};

/**
 * @}
 */

} // namespace Backbone

} // namespace otbr

#endif // BACKBONE_AGENT_HPP_
