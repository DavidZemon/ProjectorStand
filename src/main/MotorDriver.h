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
                     const Pin::Mask motorEnablePinMask)
                : m_motorDutyCycle(motorDutyCyclePinMask, Pin::Dir::OUT),
                  m_motorDirection(motorDirectionPinMask, Pin::Dir::OUT),
                  m_motorEnable(motorEnablePinMask, Pin::Dir::OUT) {
            this->m_motorEnable.clear();
            this->m_motorDutyCycle.clear();
            this->m_motorDirection.clear();
        }

        virtual void raise (const uint16_t speed) const {

        }

        virtual void drop (const uint16_t speed) const {

        }

    public:
        const Pin m_motorDutyCycle;
        const Pin m_motorDirection;
        const Pin m_motorEnable;
};
