#include <PropWare/PropWareTests.h>
#undef private
#undef protected


#include <ButtonReader.h>
#include <PropWare/concurrent/runnable.h>
#include <PropWare/utility/collection/queue.h>

using PropWare::Runnable;
using PropWare::Queue;

static const Pin::Mask UP_BUTTON_MASK   = Pin::P0;
static const Pin::Mask DOWN_BUTTON_MASK = Pin::P1;

static uint16_t        raiseBuffer[8];
static uint16_t        dropBuffer[8];
static Queue<uint16_t> raiseCalls(raiseBuffer);
static Queue<uint16_t> dropCalls(dropBuffer);

class MockAdcWrapper: public AdcWrapper {
    public:
        MockAdcWrapper ()
                : AdcWrapper(NULL, MCP3xxx::Channel::CHANNEL_0) {
        }

        virtual uint16_t read () {
            return 42;
        }
};

class MockMotorDriver: public MotorDriver {
    public:
        MockMotorDriver ()
                : MotorDriver(Pin::NULL_PIN, Pin::NULL_PIN, Pin::NULL_PIN) {
        }

        void raise (const uint16_t speed) const {
            raiseCalls.enqueue(speed);
        }

        void drop (const uint16_t speed) const {
            dropCalls.enqueue(speed);
        }
};

class ButtonReaderTest {
    public:
        ButtonReaderTest ()
                : motorDriver(),
                  testable(testableStack,
                           UP_BUTTON_MASK,
                           DOWN_BUTTON_MASK,
                           this->adc,
                           this->motorDriver,
                           testableReady),
                  testableReady(false),
                  testableCogId(Runnable::invoke(testable)) {
            raiseCalls.clear();
            dropCalls.clear();
        }

        ~ButtonReaderTest () {
            cogstop(this->testableCogId);
        }

    public:
        uint32_t           testableStack[64];
        MockAdcWrapper     adc;
        MockMotorDriver    motorDriver;
        const ButtonReader testable;
        volatile bool      testableReady;
        const int          testableCogId;
};

TEST_F(ButtonReaderTest, upButtonPushed_motorMovesUp) {
    const Pin button(UP_BUTTON_MASK, Pin::Dir::OUT);
    button.clear();

    while (!testableReady)
        waitcnt(CNT + MILLISECOND);

    // Nothing should have happened while button hasn't been pushed
    waitcnt(CNT + 3 * MILLISECOND);
    ASSERT_EQ_MSG(0, raiseCalls.size());
    ASSERT_EQ_MSG(0, dropCalls.size());

    button.set();
    waitcnt(CNT + ButtonReader::BUTTON_WAIT_PERIOD_US * MICROSECOND);
    button.clear();
    waitcnt(CNT + MILLISECOND);

    ASSERT_EQ_MSG(1, raiseCalls.size());
    ASSERT_EQ_MSG(0, dropCalls.size());
    ASSERT_EQ_MSG(42, raiseCalls.dequeue());
}

TEST_F(ButtonReaderTest, downButtonPushed_motorMovesDown) {
    const Pin button(DOWN_BUTTON_MASK, Pin::Dir::OUT);
    button.clear();

    while (!testableReady)
        waitcnt(CNT + MILLISECOND);

    // Nothing should have happened while button hasn't been pushed
    waitcnt(CNT + 3 * MILLISECOND);
    ASSERT_EQ_MSG(0, raiseCalls.size());
    ASSERT_EQ_MSG(0, dropCalls.size());

    button.set();
    waitcnt(CNT + ButtonReader::BUTTON_WAIT_PERIOD_US * MICROSECOND);
    button.clear();
    waitcnt(CNT + MILLISECOND);

    ASSERT_EQ_MSG(0, raiseCalls.size());
    ASSERT_EQ_MSG(1, dropCalls.size());
    ASSERT_EQ_MSG(42, dropCalls.dequeue());
}

int main () {
    START(ButtonReaderTest);

    RUN_TEST_F(ButtonReaderTest, upButtonPushed_motorMovesUp);
    RUN_TEST_F(ButtonReaderTest, downButtonPushed_motorMovesDown);

    COMPLETE();
}
