#include <gtest/gtest.h>
#include <Sample/HitCounterManager.h>

using namespace Sample;

TEST(HitCounterManager, GetCountForNullActor) {
    ASSERT_FALSE(HitCounterManager::GetSingleton().GetHitCount(nullptr).has_value());
}
