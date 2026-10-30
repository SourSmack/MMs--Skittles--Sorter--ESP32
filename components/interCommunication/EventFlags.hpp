// EventGroup.hpp
#pragma once

#include "etl/optional.h"
#include "etl/array.h"
#include <cstdint>

template <class GroupType>
class EventGroup {
private:
    GroupType &eventGroup_t;

public:
    static uint32_t MAX_DELAY;

    static etl::optional<EventGroup> create(GroupType &p_eventGroup, uint32_t bits);

    EventGroup(GroupType &p_eventGroup);

    bool waitBits(uint32_t message, uint32_t delay);

    static bool setBits(uint32_t message);
};

