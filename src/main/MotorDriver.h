/**
 * @file    MotorDriver
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/gpio/pin.h>

using PropWare::Pin;

class MotorDriver {
    public:
        MotorDriver (const Pin::Mask motorDutyCyclePinMask, const Pin::Mask motorDirectionPinMask,
                     const Pin::Mask motorEnablePinMask, const uint32_t frequency)
                : m_motorDutyCycle(motorDutyCyclePinMask, Pin::Dir::OUT),
                  m_motorDirection(motorDirectionPinMask, Pin::Dir::OUT),
                  m_motorEnable(motorEnablePinMask, Pin::Dir::OUT),
                  m_frequency(frequency) {
            this->m_motorEnable.clear();
            this->m_motorDutyCycle.clear();
            this->m_motorDirection.clear();
        }

        virtual void raise (const uint8_t duty) const {

        }

        virtual void drop (const uint8_t duty) const {

        }

    public:
        const Pin      m_motorDutyCycle;
        const Pin      m_motorDirection;
        const Pin      m_motorEnable;
        const uint32_t m_frequency;
};
