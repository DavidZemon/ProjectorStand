#include "CurrentWatchDog.h"
#include "ButtonReader.h"
#include "MotorDriver.h"
#include "AdcWrapper.h"

using PropWare::SPI;

static const Pin::Mask CURRENT_LIMIT_PIN_MASK = Pin::P0;

static const MCP3xxx::Channel ADC_CHANNEL = MCP3xxx::Channel::CHANNEL_0;
static const Pin::Mask        MOSI_MASK   = Pin::P1;
static const Pin::Mask        MISO_MASK   = Pin::P2;
static const Pin::Mask        SCLK_MASK   = Pin::P3;
static const Pin::Mask        CS_MASK     = Pin::P4;

static const Pin::Mask UP_BUTTON_MASK   = Pin::P5;
static const Pin::Mask DOWN_BUTTON_MASK = Pin::P6;

static const Pin::Mask MOTOR_DUTY_CYCLE_PIN_MASK = Pin::P7;
static const Pin::Mask MOTOR_DIRECTION_PIN_MASK  = Pin::P8;
static const Pin::Mask MOTOR_ENABLE_PIN_MASK     = Pin::P9;


int main () {
    uint32_t              watchDogStack[32];
    volatile bool         watchDogReady = false;
    const CurrentWatchDog currentWatchDog(watchDogStack, CURRENT_LIMIT_PIN_MASK, watchDogReady);
    Runnable::invoke(currentWatchDog);

    while (!watchDogReady)
        waitcnt(CNT + MILLISECOND);

    MCP3xxx adc(SPI::get_instance(), CS_MASK, MCP3xxx::PartNumber::MCP320x);
    SPI::get_instance().set_mosi(MOSI_MASK);
    SPI::get_instance().set_miso(MISO_MASK);
    SPI::get_instance().set_sclk(SCLK_MASK);
    AdcWrapper adcWrapper(&adc, ADC_CHANNEL)

    const MotorDriver motorDriver(MOTOR_DUTY_CYCLE_PIN_MASK, MOTOR_DIRECTION_PIN_MASK, MOTOR_ENABLE_PIN_MASK);

    const ButtonReader buttonReader(UP_BUTTON_MASK, DOWN_BUTTON_MASK, adcWrapper, motorDriver);
    return buttonReader.run();
}
