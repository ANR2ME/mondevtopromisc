#pragma once

/* Copyright (c) 2020 [Rick de Bondt] - IPCapDevice.h
 *
 * This file contains an interface for pcap devices, either file based or device based.
 *
 **/

#include <string>
#include <vector>

#include <pcap/pcap.h>

#include "ISendReceiveDevice.h"
#include "NetworkingHeaders.h"

namespace IPCapDevice_Constants
{
    struct WiFiBeaconInformation
    {
        uint64_t    BSSID{};
        std::string SSID{};
        uint8_t     MaxRate{RadioTap_Constants::cRateFlags};
        uint16_t    Frequency{RadioTap_Constants::cChannel};
    };
}  // namespace IPCapDevice_Constants


/**
 * Interface for pcap devices, either file based or device based.
 */
class IPCapDevice : public ISendReceiveDevice
{
public:
    /**
     * Opens the PCAP device so it can be used for capture.
     * @param aName - Name of the interface or file to use.
     * @param aSSIDFilter - The SSIDS to listen to.
     * @param aFrequency - The frequency to listen to initially.
     * @return true if successful.
     */
    virtual bool Open(std::string_view aName, std::vector<std::string>& aSSIDFilter, uint16_t aFrequency) = 0;

    /**
     * Returns data as string.
     * @param aData - Data from pcap functions.
     * @param aHeader - Header from pcap functions.
     * @return Data as string.
     */
    virtual std::string DataToString(const unsigned char* aData, const pcap_pkthdr* aHeader) = 0;

    /**
     * Gets data from last read packet.
     * @return pointer to data as an unsigned char array.
     */
    virtual const unsigned char* GetData() = 0;

    /**
     * Gets header from last read packet.
     * @return pointer to header as pcap_pkthdr type.
     */
    virtual const pcap_pkthdr* GetHeader() = 0;

    /**
     * Sends data over device/file if supported.
     * @param aData - Data to send.
     * @param aWirelessInformation - Information needed to send monitor mode information.
     * @return true if successful, false on failure or unsupported.
     */
    virtual bool Send(std::string_view aData, IPCapDevice_Constants::WiFiBeaconInformation& aWirelessInformation) = 0;
};
