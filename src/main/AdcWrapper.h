/**
 * @file    AdcWrapper
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/sensor/analog/mcp3xxx.h>

using PropWare::MCP3xxx;

class AdcWrapper {
    public:
        AdcWrapper (MCP3xxx *adc, const MCP3xxx::Channel channel)
                : m_adc(adc),
                  m_channel(channel) {
        }

        virtual uint16_t read () {
            return this->m_adc->read(this->m_channel);
        }

    private:
        MCP3xxx                *m_adc;
        const MCP3xxx::Channel m_channel;
};
