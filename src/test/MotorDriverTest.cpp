/**
 * @file    MotorDriverTest
 *
 * @author  David Zemon
 */

#include <PropWare/PropWareTests.h>

#include <MotorDriver.h>
#include <PropWare/concurrent/runnable.h>

using PropWare::Runnable;

class MotorDriverRunnable: public Runnable {
    public:
        static const Pin::Mask DUTY_CYCLE_MASK         = Pin::Mask::P0;
        static const Pin::Mask DIRECTION_MASK          = Pin::Mask::P1;
        static const Pin::Mask ENABLE_MASK             = Pin::Mask::P2;
        static const uint32_t  FREQUENCY               = 1000;
        static const Pin::Mask RAISE_LIMIT_SWITCH_MASK = Pin::Mask::P3;
        static const Pin::Mask DROP_LIMIT_SWITCH_MASK  = Pin::Mask::P4;

    public:
        template<size_t N>
        MotorDriverRunnable (const uint32_t (&stack)[N], volatile uint8_t &raiseDuty, volatile uint8_t &dropDuty,
                             volatile bool &ready)
                : Runnable(stack),
                  m_raiseDuty(&raiseDuty),
                  m_dropDuty(&dropDuty),
                  ready(&ready) {
        }

        void run () {
            const MotorDriver testable(DUTY_CYCLE_MASK, DIRECTION_MASK, ENABLE_MASK, FREQUENCY, RAISE_LIMIT_SWITCH_MASK,
                                       DROP_LIMIT_SWITCH_MASK);
            *this->ready = true;
            while (1) {
                if (*this->m_raiseDuty)
                    testable.raise(*this->m_raiseDuty);
                if (*this->m_dropDuty)
                    testable.drop(*this->m_dropDuty);
            }
        }

    private:
        volatile uint8_t *m_raiseDuty;
        volatile uint8_t *m_dropDuty;
        volatile bool    *ready;
};

class MotorDriverTest {
    public:
        MotorDriverTest ()
                : raiseDuty(0),
                  dropDuty(0),
                  ready(false),
                  testRunnable(this->stack, this->raiseDuty, this->dropDuty, this->ready),
                  cogId(Runnable::invoke(testRunnable)),
                  dutyCyclePin(MotorDriverRunnable::DUTY_CYCLE_MASK, Pin::Dir::IN),
                  directionPin(MotorDriverRunnable::DIRECTION_MASK, Pin::Dir::IN),
                  enablePin(MotorDriverRunnable::ENABLE_MASK, Pin::Dir::IN),
                  raiseLimitSwitch(MotorDriverRunnable::RAISE_LIMIT_SWITCH_MASK, Pin::Dir::OUT),
                  dropLimitSwitch(MotorDriverRunnable::DROP_LIMIT_SWITCH_MASK, Pin::Dir::OUT) {
            this->raiseLimitSwitch.set();
            this->dropLimitSwitch.set();

            while (!this->ready)
                waitcnt(CNT + MILLISECOND);
        }

        ~MotorDriverTest () {
            cogstop(this->cogId);
        }

    public:
        volatile uint8_t raiseDuty;
        volatile uint8_t dropDuty;
        volatile bool    ready;

        uint32_t            stack[128];
        MotorDriverRunnable testRunnable;
        int                 cogId;

        const Pin dutyCyclePin;
        const Pin directionPin;
        const Pin enablePin;
        const Pin raiseLimitSwitch;
        const Pin dropLimitSwitch;
};

TEST_F(MotorDriverTest, raise_limitSwitchAlreadySet_noAction) {
    // The motor shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());
    waitcnt(CNT + 5 * MILLISECOND);
    // The motor really shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());

    this->raiseLimitSwitch.clear();
    this->raiseDuty = 10;
    waitcnt(CNT + MILLISECOND);
    ASSERT_FALSE(this->enablePin.read());
}

TEST_F(MotorDriverTest, drop_limitSwitchAlreadySet_noAction) {
    // The motor shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());
    waitcnt(CNT + 5 * MILLISECOND);
    // The motor really shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());

    this->dropLimitSwitch.clear();
    this->dropDuty = 10;
    waitcnt(CNT + MILLISECOND);
    ASSERT_FALSE(this->enablePin.read());
}

TEST_F(MotorDriverTest, raise) {
    // The motor shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());
    waitcnt(CNT + 5 * MILLISECOND);
    // The motor really shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());

    this->raiseDuty = 90;
    waitcnt(CNT + MILLISECOND);
    ASSERT_TRUE(this->enablePin.read());

    // Set the stage for PWM measurement
    this->dutyCyclePin.wait_until_high();

    // Verify PWM signal and constant direction pin
    this->dutyCyclePin.wait_until_low();
    uint32_t timer = CNT;
    ASSERT_FALSE(this->directionPin.read());

    this->dutyCyclePin.wait_until_high();
    uint32_t lowTime = PropWare::Utility::measure_time_interval(timer);
    timer = CNT;
    ASSERT_BETWEEN_INC_MSG(85, lowTime, 115);
    ASSERT_FALSE(this->directionPin.read());

    this->dutyCyclePin.wait_until_low();
    uint32_t highTime = PropWare::Utility::measure_time_interval(timer);
    timer = CNT;
    ASSERT_BETWEEN_INC_MSG(885, highTime, 915);
    ASSERT_FALSE(this->directionPin.read());

    this->dutyCyclePin.wait_until_high();
    lowTime = PropWare::Utility::measure_time_interval(timer);
    timer   = CNT;
    ASSERT_BETWEEN_INC_MSG(85, lowTime, 115);
    ASSERT_FALSE(this->directionPin.read());

    this->dutyCyclePin.wait_until_low();
    highTime = PropWare::Utility::measure_time_interval(timer);
    ASSERT_BETWEEN_INC_MSG(885, highTime, 915);
    ASSERT_FALSE(this->directionPin.read());

    // Limit switch is hit, motor should stop
    this->raiseLimitSwitch.clear();
    waitcnt(CNT + MILLISECOND);
    ASSERT_FALSE(this->enablePin.read());
}

TEST_F(MotorDriverTest, lower) {
    // The motor shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());
    waitcnt(CNT + 5 * MILLISECOND);
    // The motor really shouldn't be turning yet...
    ASSERT_FALSE(this->enablePin.read());

    this->dropDuty = 90;
    waitcnt(CNT + MILLISECOND);
    ASSERT_TRUE(this->enablePin.read());

    // Set the stage for PWM measurement
    this->dutyCyclePin.wait_until_high();

    // Verify PWM signal and constant direction pin
    this->dutyCyclePin.wait_until_low();
    uint32_t timer = CNT;
    ASSERT_TRUE(this->directionPin.read());

    this->dutyCyclePin.wait_until_high();
    uint32_t lowTime = PropWare::Utility::measure_time_interval(timer);
    timer = CNT;
    ASSERT_BETWEEN_INC_MSG(85, lowTime, 115);
    ASSERT_TRUE(this->directionPin.read());

    this->dutyCyclePin.wait_until_low();
    uint32_t highTime = PropWare::Utility::measure_time_interval(timer);
    timer = CNT;
    ASSERT_BETWEEN_INC_MSG(885, highTime, 915);
    ASSERT_TRUE(this->directionPin.read());

    this->dutyCyclePin.wait_until_high();
    lowTime = PropWare::Utility::measure_time_interval(timer);
    timer   = CNT;
    ASSERT_BETWEEN_INC_MSG(85, lowTime, 115);
    ASSERT_TRUE(this->directionPin.read());

    this->dutyCyclePin.wait_until_low();
    highTime = PropWare::Utility::measure_time_interval(timer);
    ASSERT_BETWEEN_INC_MSG(885, highTime, 915);
    ASSERT_TRUE(this->directionPin.read());

    // Limit switch is hit, motor should stop
    this->raiseLimitSwitch.clear();
    waitcnt(CNT + MILLISECOND);
    ASSERT_FALSE(this->enablePin.read());
}

int main () {
    START(MotorDriverTest);

    RUN_TEST_F(MotorDriverTest, raise_limitSwitchAlreadySet_noAction);
    RUN_TEST_F(MotorDriverTest, drop_limitSwitchAlreadySet_noAction);
    RUN_TEST_F(MotorDriverTest, raise);
    RUN_TEST_F(MotorDriverTest, lower);

    COMPLETE();
}
