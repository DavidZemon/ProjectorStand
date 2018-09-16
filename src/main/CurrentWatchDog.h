/**
 * @file    CurrentWatchDog
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/concurrent/runnable.h>
#include <PropWare/gpio/pin.h>

using PropWare::Runnable;
using PropWare::Pin;

class CurrentWatchDog: public Runnable {
    public:
        template<size_t N>
        CurrentWatchDog (const uint32_t (&stack)[N], const Pin::Mask pinMask, volatile bool &readySignal)
                : Runnable(stack),
                  m_pin(pinMask),
                  m_readySignal(&readySignal) {
        }

        virtual void run () {
            this->m_pin.set_dir_in();
            *this->m_readySignal = true;

            this->m_pin.wait_until_high();

            const int currentId = cogid();
            for (int i = 0; i < 8; ++i)
                if (currentId != i)
                    cogstop(i);
        }

    private:
        const Pin m_pin;
        volatile bool *m_readySignal;
};
