#include <PropWare/PropWareTests.h>
#undef cogstop

void cogstop (const int cogId);

#include <CurrentWatchDog.h>
#include <vector>
#include <algorithm>

static std::vector<int> stoppedCogs;

void cogstop (const int cogId) {
    stoppedCogs.push_back(cogId);
}

static const PropWare::Port::Mask PIN_MASK = Pin::P0;
class CurrentWatchDogTest {
    public:
        CurrentWatchDogTest ()
                : watchDogReady(false),
                  testable(this->m_stack, PIN_MASK, watchDogReady),
                  pin(PIN_MASK, Pin::Dir::OUT) {
            stoppedCogs.clear();
            pin.clear();
        }

    public:
        uint32_t        m_stack[64];
        volatile bool   watchDogReady;
        CurrentWatchDog testable;
        Pin             pin;
};

TEST_F(CurrentWatchDogTest, WillStopWhenPinGoesHigh) {
    const int8_t cogWithTestable = Runnable::invoke(testable);
    while (!watchDogReady)
        waitcnt(CNT + 10 * MILLISECOND);

    waitcnt(CNT + 10 * MILLISECOND);
    ASSERT_EQ_MSG(0, stoppedCogs.size());

    pin.set();
    waitcnt(CNT + 2 * MILLISECOND);

    ASSERT_EQ_MSG(7, stoppedCogs.size());
    for (int i = 0; i < 8; ++i)
        if (cogWithTestable != i)
            ASSERT_NEQ(stoppedCogs.end(), std::find(stoppedCogs.begin(), stoppedCogs.end(), i));
}

int main () {
    START(CurrentWatchDogTest);

    RUN_TEST_F(CurrentWatchDogTest, WillStopWhenPinGoesHigh);

    COMPLETE();
}
