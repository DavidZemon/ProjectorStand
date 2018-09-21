/**
 * @file    CurrentWatchDog
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/concurrent/runnable.h>
#include <PropWare/gpio/pin.h>
#include <PropWare/hmi/output/ws2812.h>

using PropWare::Runnable;
using PropWare::Pin;
using PropWare::WS2812;

class CurrentWatchDog: public Runnable {
    public:
        template<size_t N>
        CurrentWatchDog (const uint32_t (&stack)[N], const Pin::Mask currentSensePinMask,
                         const Pin::Mask ledPinMask, volatile bool &readySignal)
                : Runnable(stack),
                  m_currentSensePin(currentSensePinMask),
                  m_ledPinMask(ledPinMask),
                  m_readySignal(&readySignal) {
        }

        virtual void run () {
            const WS2812 errorLed(this->m_ledPinMask, WS2812::Type::RGB);

            this->m_currentSensePin.set_dir_in();
            *this->m_readySignal = true;

            this->m_currentSensePin.wait_until_high();

            const int currentId = cogid();
            for (int  i         = 0; i < 8; ++i)
                if (currentId != i)
                    cogstop(i);

            while (1) {
                errorLed.send(WS2812::Color::RED);
                waitcnt(CNT + 125 * MILLISECOND);
                errorLed.send(WS2812::Color::BLACK);
                waitcnt(CNT + 125 * MILLISECOND);
            }
        }

    private:
        const Pin       m_currentSensePin;
        const Pin::Mask m_ledPinMask;
        volatile bool   *m_readySignal;
};
