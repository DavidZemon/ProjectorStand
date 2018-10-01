/**
 * @file    PropWare/sensor/analog/mcp3xxx.h
 *
 * @author  David Zemon
 * @author  Collin Winans
 *
 * @copyright
 * The MIT License (MIT)<br>
 * <br>Copyright (c) 2013 David Zemon<br>
 * <br>Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:<br>
 * <br>The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.<br>
 * <br>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <PropWare/PropWare.h>
#include <PropWare/serial/spi/spi.h>
#include <PropWare/sensor/analog/mcp3xxx.h>

using PropWare::Pin;
using PropWare::SPI;
using PropWare::MCP3xxx;

class SinglePinMCP3208: public MCP3xxx {
    public:
        SinglePinMCP3208 (const Pin::Mask dataPinMask, const Pin::Mask sclkMask, const PropWare::Pin::Mask cs)
                : PropWare::MCP3xxx(SPI::get_instance(), cs, MCP3xxx::PartNumber::MCP320x),
                  m_dataPin(dataPinMask) {
            SPI::get_instance().set_mosi(dataPinMask);
            SPI::get_instance().set_miso(dataPinMask);
            SPI::get_instance().set_sclk(sclkMask);
        }

        uint16_t read (const SinglePinMCP3208::Channel channel) {
            int8_t   options;
            uint16_t dat;

            options = START | static_cast<int8_t>(SINGLE_ENDED) | static_cast<int8_t>(channel);

            // Two dead bits between output and input - see page 19 of datasheet
            options <<= 2;

            this->m_cs.clear();
            this->m_dataPin.set_dir_out();
            this->m_spi->shift_out(OPTION_WIDTH, (uint32_t) options);
            this->m_dataPin.set_dir_in();
            dat = (uint16_t) this->m_spi->shift_in(this->m_dataWidth);
            this->m_cs.set();

            return dat;
        }

    private:
        const Pin m_dataPin;
};
