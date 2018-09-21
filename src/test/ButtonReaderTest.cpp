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

static uint8_t        raiseBuffer[8];
static uint8_t        dropBuffer[8];
static Queue<uint8_t> raiseCalls(raiseBuffer);
static Queue<uint8_t> dropCalls(dropBuffer);

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
                : MotorDriver(Pin::NULL_PIN, Pin::NULL_PIN, Pin::NULL_PIN, 0, Pin::NULL_PIN, Pin::NULL_PIN) {
        }

        void raise (const uint8_t speed) const {
            raiseCalls.enqueue(speed);
        }

        void drop (const uint8_t speed) const {
            dropCalls.enqueue(speed);
        }
};

class ButtonReaderWrapper: public Runnable {
    public:
        template<size_t N>
        ButtonReaderWrapper (const uint32_t (&stack)[N], volatile bool &ready, MockAdcWrapper &adc,
                             MockMotorDriver &motorDriver)
                :Runnable(stack),
                 m_ready(&ready),
                 testable(UP_BUTTON_MASK,
                          DOWN_BUTTON_MASK,
                          adc,
                          motorDriver) {
        }

        void run () {
            *this->m_ready = true;
            this->testable.run();
        }

    public:
        volatile bool      *m_ready;
        const ButtonReader testable;
};

class ButtonReaderTest {
    public:
        ButtonReaderTest ()
                : motorDriver(),
                  testableReady(false),
                  testableWrapper(testableStack, testableReady, this->adc, this->motorDriver),
                  testableCogId(Runnable::invoke(this->testableWrapper)) {
            raiseCalls.clear();
            dropCalls.clear();
        }

        ~ButtonReaderTest () {
            cogstop(this->testableCogId);
        }

    public:
        uint32_t            testableStack[64];
        MockAdcWrapper      adc;
        MockMotorDriver     motorDriver;
        volatile bool       testableReady;
        ButtonReaderWrapper testableWrapper;
        const int           testableCogId;
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
