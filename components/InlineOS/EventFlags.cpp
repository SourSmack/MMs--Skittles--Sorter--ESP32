#pragma once
#include "EventGroup.hpp"
#include "freertos/event_groups.h"

template <class GroupType>
uint32_t EventGroup<GroupType>::MAX_DELAY = portMAX_DELAY;

template <class GroupType>
etl::optional<EventGroup<GroupType>> EventGroup<GroupType>::create(GroupType &p_eventGroup, uint32_t bits) {
    return etl::optional<EventGroup<GroupType>>{ etl::in_place, p_eventGroup };
}

template <class GroupType>
EventGroup<GroupType>::EventGroup(GroupType &p_eventGroup) 
    : eventGroup_t(p_eventGroup) {}

template <class GroupType>
bool EventGroup<GroupType>::waitBits(uint32_t message, uint32_t delay) {
    auto res = xEventGroupWaitBits(eventGroup_t, message, message, pdTRUE, delay);
    if ((message & res) == 0) return false;
    return true;
}

template <class GroupType>
bool EventGroup<GroupType>::setBits(uint32_t message) {
    auto res = xEventGroupSetBits(eventGroup_t, message);
    if ((message & res) == 0) return false;
    return true;
}