/**
 * @file    ProjectorStand
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/concurrent/runnable.h>
#include <PropWare/gpio/pin.h>
#include <PropWare/gpio/simpleport.h>
#include <AdcWrapper.h>
#include "MotorDriver.h"

using PropWare::Pin;
using PropWare::MCP3xxx;
using PropWare::Runnable;

class ButtonReader: public Runnable {
    public:
        static const uint32_t BUTTON_WAIT_PERIOD_US = 400;

    public:
        template<size_t N>
        ButtonReader (const uint32_t (&stack)[N],
                      const Pin::Mask upButtonMask,
                      const Pin::Mask downButtonMask,
                      AdcWrapper &adc,
                      MotorDriver &motorDriver,
                      volatile bool &ready)
                : Runnable(stack),
                  m_upButton(upButtonMask, Pin::Dir::IN),
                  m_downButton(downButtonMask, Pin::Dir::IN),
                  m_adc(&adc),
                  m_motorDriver(&motorDriver),
                  m_ready(&ready) {
        }

        void run () {
            this->m_upButton.set_dir_in();
            this->m_downButton.set_dir_in();
            *this->m_ready = true;

            while (true) {
                uint16_t adcValue;
                do {
                    adcValue = this->m_adc->read();
                    waitcnt(CNT + BUTTON_WAIT_PERIOD_US * MICROSECOND);
                } while (!this->m_upButton.read() && !this->m_downButton.read());

                if (this->m_upButton.read()) {
                    this->m_motorDriver->raise(adcValue);
                } else {
                    this->m_motorDriver->drop(adcValue);
                }
            }
        }

    public:
        const Pin     m_upButton;
        const Pin     m_downButton;
        AdcWrapper    *m_adc;
        MotorDriver   *m_motorDriver;
        volatile bool *m_ready;
};
