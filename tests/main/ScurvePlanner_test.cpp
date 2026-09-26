#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"





class ScurveTesting : public ::testing::Test {
protected:

    void SetUp() override {
    }

    void TearDown() override {
};

using  Scurve_waitMs_test = ScurveTesting ;

using testing::_; 
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgPointee;


TEST_F


