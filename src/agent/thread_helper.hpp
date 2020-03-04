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

#ifndef OTBR_THREAD_HELPER_HPP_
#define OTBR_THREAD_HELPER_HPP_

#include <chrono>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/jam_detection.h>
#include <openthread/joiner.h>
#include <openthread/netdata.h>
#include <openthread/thread.h>

namespace otbr {
namespace Ncp {
class ControllerOpenThread;
}
} // namespace otbr

namespace otbr {
namespace agent {

class ThreadHelper
{
public:
    using DeviceRoleHandler = std::function<void(otDeviceRole)>;
    using ScanHandler       = std::function<void(otError, const std::vector<otActiveScanResult> &)>;
    using ResultHandler     = std::function<void(otError)>;

    /**
     * The constructor of a Thread helper.
     *
     * @param[in]   aInstance  The Thread instance.
     * @param[in]   aNcp       The ncp controller.
     *
     */
    ThreadHelper(otInstance *aInstance, otbr::Ncp::ControllerOpenThread *aNcp);

    /**
     * This method initializes the Thread helper.
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError Init(void);

    /**
     * This method adds a callback for device role change.
     *
     * @param[in]   aHandler  The device role handler.
     *
     */
    void AddDeviceRoleHandler(DeviceRoleHandler aHandler);

    /**
     * This method adds an unsecure thread port.
     *
     * @param[in]   aPort     The port number.
     * @param[in]   aSeconds  The timeout to close the port, 0 for never close.
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError AddUnsecurePort(uint16_t aPort, uint32_t aSeconds);

    /**
     * This method performs a Thread network scan.
     *
     * @param[in]   aHandler  The scan result handler.
     *
     */
    void Scan(ScanHandler aHandler);

    /**
     * This method attaches the device to the Thread network.
     *
     * @note The joiner start and the attach proccesses are exclusive
     *
     * @param[in]   aNetworkName    The network name.
     * @param[in]   aPanId          The pan id, UINT16_MAX for random.
     * @param[in]   aExtPanId       The extended pan id, UINT64_MAX for random.
     * @param[in]   aMasterKey      The master key, empty for random.
     * @param[in]   aPSKc           The pre-shared commissioner key, empty for random.
     * @param[in]   aChannelMask    A bitmask for valid channels, will random select one.
     * @param[in]   aHandler        The attach result handler.
     *
     */
    void Attach(const std::string &         aNetworkName,
                uint16_t                    aPanId,
                uint64_t                    aExtPanId,
                const std::vector<uint8_t> &aMasterKey,
                const std::vector<uint8_t> &aPSKc,
                uint32_t                    aChannelMask,
                ResultHandler               aHandler);

    /**
     * This method resets the OpenThread stack.
     *
     * @returns The error value of underlying OpenThread api calls.
     *
     */
    otError Reset(void);

    /**
     * This method triggers a thread join process.
     *
     * @note The joiner start and the attach proccesses are exclusive
     *
     * @param[in]   aPskd             The pre-shared key for device.
     * @param[in]   aProvisioningUrl  The provision url.
     * @param[in]   aVendorName       The vendor name.
     * @param[in]   aVendorModel      The vendor model.
     * @param[in]   aVendorSwVersion  The vendor software version.
     * @param[in]   aVendorData       The vendor custom data.
     * @param[in]   aHandler          The join result handler.
     *
     */
    void JoinerStart(const std::string &aPskd,
                     const std::string &aProvisioningUrl,
                     const std::string &aVendorName,
                     const std::string &aVendorModel,
                     const std::string &aVendorSwVersion,
                     const std::string &aVendorData,
                     ResultHandler      aHandler);

    /**
     * This method returns the underlying OpenThread instance.
     *
     * @returns The underlying instance.
     *
     */
    otInstance *GetInstance(void) { return mInstance; }

private:
    static void sStateChangedCallback(otChangedFlags aFlags, void *aThreadHelper);
    void        StateChangedCallback(otChangedFlags aFlags);

    static void sActiveScanHandler(otActiveScanResult *aResult, void *aThreadHelper);
    void        ActiveScanHandler(otActiveScanResult *aResult);

    static void sJoinerCallback(otError aError, void *aThreadHelper);
    void        JoinerCallback(otError aResult);

    void    RandomFill(void *aBuf, size_t size);
    uint8_t RandomChannelFromChannelMask(uint32_t aChannelMask);

    otInstance *mInstance;

    otbr::Ncp::ControllerOpenThread *mNcp;

    ScanHandler                     mScanHandler;
    std::vector<otActiveScanResult> mScanResults;

    std::vector<DeviceRoleHandler> mDeviceRoleHandlers;

    std::map<uint16_t, std::chrono::steady_clock::time_point> mPortTime;

    ResultHandler mAttachHandler;
    ResultHandler mJoinerHandler;

    std::random_device mRandomDevice;
};

} // namespace agent
} // namespace otbr

#endif // OTBR_THREAD_HELPER_HPP_
