/**
 * @file    MotorDriver
 *
 * @author  David Zemon
 */

#pragma once

#include <PropWare/gpio/pin.h>
#include <PropWare/gpio/dualpwm.h>

using PropWare::Pin;
using PropWare::DualPWM;

class MotorDriver {
    public:
        MotorDriver (const Pin::Mask motorPwmPinMask, const Pin::Mask motorDirectionPinMask,
                     const Pin::Mask motorEnablePinMask, const uint32_t frequency, const Pin::Mask raiseLimitSwitchMask,
                     const Pin::Mask dropLimitSwitchMask)
                : m_pwmPin(motorPwmPinMask, Pin::Dir::OUT),
                  m_motorDirection(motorDirectionPinMask, Pin::Dir::OUT),
                  m_motorEnable(motorEnablePinMask, Pin::Dir::OUT),
                  m_period(CLKFREQ / frequency),
                  m_raiseLimitSwitch(raiseLimitSwitchMask, Pin::Dir::IN),
                  m_dropLimitSwitch(dropLimitSwitchMask, Pin::Dir::IN) {
            this->m_pwmPin.clear();
            this->m_motorEnable.clear();
            this->m_motorDirection.clear();

            CTRA = 0b00100 << 26 | static_cast<unsigned int>(this->m_pwmPin.get_pin_number());
            FRQA = 1;
        }

        ~MotorDriver () {
            this->m_pwmPin.set_dir_in();
            this->m_motorDirection.set_dir_in();
            this->m_motorEnable.set_dir_in();
        }

        virtual void raise (const uint8_t duty) const {
            if (this->m_raiseLimitSwitch.read())
                return;
            else {
                this->m_motorDirection.clear();
                this->do_pwm(duty);
            }
        }

        virtual void drop (const uint8_t duty) const {
            if (this->m_dropLimitSwitch.read())
                return;
            else {
                this->m_motorDirection.set();
                this->do_pwm(duty);
            }
        }

    private:
        void do_pwm (const uint8_t duty) const {
            const auto phase = this->m_period * duty / 100;

            this->m_motorEnable.set();
            auto timer = CNT + this->m_period;
            while (!this->m_raiseLimitSwitch.read()) {
                __asm__ volatile("neg PHSA, %0" : : "r" (phase));
                timer = waitcnt2(timer, this->m_period);
            }

            this->m_motorEnable.clear();
        }

    private:
        const Pin          m_pwmPin;
        const Pin          m_motorDirection;
        const Pin          m_motorEnable;
        const unsigned int m_period;
        const Pin          m_raiseLimitSwitch;
        const Pin          m_dropLimitSwitch;
};
