#include <Sample/HitCounterManager.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace Sample;

TEST_CASE("HitCounterManager/GetCountForNullActor") {
    CHECK(!HitCounterManager::GetSingleton().GetHitCount(nullptr).has_value());
}
