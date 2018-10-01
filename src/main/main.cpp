#include "CurrentWatchDog.h"
#include "ButtonReader.h"
#include "MotorDriver.h"
#include "AdcWrapper.h"

using PropWare::SPI;

static const Pin::Mask CURRENT_LIMIT_PIN_MASK = Pin::P0;
static const Pin::Mask ERROR_LED_PIN_MASK     = Pin::P1;

static const SinglePinMCP3208::Channel ADC_CHANNEL = SinglePinMCP3208::Channel::CHANNEL_0;
static const Pin::Mask                 SCLK_MASK   = Pin::P25;
static const Pin::Mask                 DATA_MASK   = Pin::P26;
static const Pin::Mask                 CS_MASK     = Pin::P27;

static const Pin::Mask UP_BUTTON_MASK   = Pin::P6;
static const Pin::Mask DOWN_BUTTON_MASK = Pin::P7;

static const Pin::Mask MOTOR_DUTY_CYCLE_PIN_MASK   = Pin::P8;
static const Pin::Mask MOTOR_DIRECTION_PIN_MASK    = Pin::P9;
static const Pin::Mask MOTOR_ENABLE_PIN_MASK       = Pin::P10; // Connect to both EN and !D2
static const uint32_t  MOTOR_PWM_FREQUENCY         = 20000; // Measured in Hz
static const Pin::Mask RAISE_LIMIT_SWITCH_PIN_MASK = Pin::P11;
static const Pin::Mask LOWER_LIMIT_SWITCH_PIN_MASK = Pin::P12;

static uint32_t watchDogStack[64];

int main () {
    volatile bool         watchDogReady = false;
    const CurrentWatchDog currentWatchDog(watchDogStack, CURRENT_LIMIT_PIN_MASK, ERROR_LED_PIN_MASK, watchDogReady);
    Runnable::invoke(currentWatchDog);

    while (!watchDogReady)
        waitcnt(CNT + MILLISECOND);

    SinglePinMCP3208 adc(DATA_MASK, SCLK_MASK, CS_MASK);
    AdcWrapper       adcWrapper(&adc, ADC_CHANNEL);

    const MotorDriver motorDriver(MOTOR_DUTY_CYCLE_PIN_MASK, MOTOR_DIRECTION_PIN_MASK, MOTOR_ENABLE_PIN_MASK,
                                  MOTOR_PWM_FREQUENCY, RAISE_LIMIT_SWITCH_PIN_MASK, LOWER_LIMIT_SWITCH_PIN_MASK);

    const ButtonReader buttonReader(UP_BUTTON_MASK, DOWN_BUTTON_MASK, adcWrapper, motorDriver);
    buttonReader.run();
    return 0;
}
