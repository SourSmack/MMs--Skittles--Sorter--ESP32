#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "Nema.hpp"
#include "mocks.hpp"

class NemaTesting : public ::testing::Test {
protected:
    PlannerMOCK planner;
    StepperMOCK stepper;
    TaskMOCK task;

    Nema<PlannerMOCK, StepperMOCK, TaskMOCK> nemaEngine{ 0, 0, planner, stepper, task };
    const motionBlock_t VALID_BLOCK { 1, 2, true };
    const motionBlock_t INVALID_BLOCK { 0, 0, false };

    void TearDown() override {
        testing::Mock::VerifyAndClearExpectations(&plannerMockImpl);
        testing::Mock::VerifyAndClearExpectations(&stepperMockImpl);
        testing::Mock::VerifyAndClearExpectations(&taskMockImpl);
    }
};

using start = NemaTesting;
using moveToCup = NemaTesting;

using ::testing::_;
using ::testing::Return;

TEST_F(start, HappyPath) {
    testing::InSequence seq;

    EXPECT_CALL(stepperMockImpl, start()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(plannerMockImpl, start()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(taskMockImpl, start()).Times(1).WillOnce(Return(true));

    auto result = nemaEngine.start();

    EXPECT_EQ(result, true);
}

TEST_F(moveToCup, HappyPathEnqueue) {
    EXPECT_CALL(stepperMockImpl, start()).WillOnce(Return(true));
    EXPECT_CALL(plannerMockImpl, start()).WillOnce(Return(true));
    EXPECT_CALL(taskMockImpl, start()).WillOnce(Return(true));

    nemaEngine.start();
    testing::InSequence seq;

    EXPECT_CALL(plannerMockImpl, enqueue(_)).Times(1);

    nemaEngine.moveToCup(moveBlock_t{ 0 }, moveInfo_t{ true, true }, 0);
}

TEST_F(moveToCup, HappyPathNotEnqueue) {
    EXPECT_CALL(stepperMockImpl, start()).WillOnce(Return(true));
    EXPECT_CALL(plannerMockImpl, start()).WillOnce(Return(true));
    EXPECT_CALL(taskMockImpl, start()).WillOnce(Return(true));

    nemaEngine.start();

    testing::InSequence seq;

    EXPECT_CALL(plannerMockImpl, stop()).Times(1);

    EXPECT_CALL(plannerMockImpl, calculateFrequency(_))
        .WillOnce(Return(VALID_BLOCK))
        .WillOnce(Return(VALID_BLOCK))
        .WillOnce(Return(VALID_BLOCK))
        .WillOnce(Return(VALID_BLOCK))
        .WillOnce(Return(INVALID_BLOCK));

    EXPECT_CALL(stepperMockImpl, enqueue(_)).Times(4);
    EXPECT_CALL(plannerMockImpl, start()).Times(1);

    nemaEngine.moveToCup(moveBlock_t{ 0 }, moveInfo_t{ false, false }, 0);
}
