/**
 * @file    AdcWrapper
 *
 * @author  David Zemon
 */

#pragma once

#include "singlepinmcp3208.h"

class AdcWrapper {
    public:
        AdcWrapper (SinglePinMCP3208 *adc, const MCP3xxx::Channel channel)
                : m_adc(adc),
                  m_channel(channel) {
        }

        virtual uint16_t read () {
            return this->m_adc->read(this->m_channel);
        }

    private:
        SinglePinMCP3208                *m_adc;
        const SinglePinMCP3208::Channel m_channel;
};
