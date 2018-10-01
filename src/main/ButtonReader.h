/**
 * @file    ProjectorStand
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/concurrent/runnable.h>
#include <PropWare/gpio/pin.h>
#include <PropWare/gpio/simpleport.h>
#include "singlepinmcp3208.h"
#include "AdcWrapper.h"
#include "MotorDriver.h"

using PropWare::Pin;
using PropWare::Runnable;

class ButtonReader {
    public:
        static const uint32_t BUTTON_WAIT_PERIOD_US = 400;
        static const uint32_t MAX_ADC_VALUE         = 4096;

    public:
        ButtonReader (const Pin::Mask upButtonMask,
                      const Pin::Mask downButtonMask,
                      AdcWrapper &adc,
                      const MotorDriver &motorDriver)
                : m_upButton(upButtonMask, Pin::Dir::IN),
                  m_downButton(downButtonMask, Pin::Dir::IN),
                  m_adc(&adc),
                  m_motorDriver(&motorDriver) {
        }

        void run () const {
            this->m_upButton.set_dir_in();
            this->m_downButton.set_dir_in();

            while (true) {
                uint16_t adcValue;
                do {
                    adcValue = this->m_adc->read();
                    waitcnt(CNT + BUTTON_WAIT_PERIOD_US * MICROSECOND);
                } while (!this->m_upButton.read() && !this->m_downButton.read());

                if (this->m_upButton.read()) {
                    this->m_motorDriver->raise(adcValue * MotorDriver::MAX_DUTY / MAX_ADC_VALUE);
                } else {
                    this->m_motorDriver->drop(adcValue * MotorDriver::MAX_DUTY / MAX_ADC_VALUE);
                }
            }
        }

    public:
        const Pin         m_upButton;
        const Pin         m_downButton;
        AdcWrapper        *m_adc;
        const MotorDriver *m_motorDriver;
};
