#include "ConceptsConfig.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "etl/optional.h"
#include <format>
#include <random>

template <class T, class N>
class EventGroupMockImpl {
public:
    MOCK_METHOD((etl::optional<T>), At, (const uint32_t idx));
    MOCK_METHOD((bool), waitBits, (const uint32_t message, const uint32_t delay));
};

template <class T, class N>
inline EventGroupMockImpl<T, N> eventGroupMockImpl;

template <class T, class N>
class EventGroupMOCK {
public:
    using GroupType = T;
    using MembersType = N;

    static etl::optional<EventGroupMOCK> create(T& p_eventGroup, const uint32_t flagsBits) {
        return EventGroupMOCK{};
    }

    etl::optional<T> At(const uint32_t idx) {
        return eventGroupMockImpl<T, N>.At(idx);
    }

    etl::optional<N> operator[](const uint32_t idx) {
        return At(idx);
    }

    bool waitBits(const uint32_t message, const uint32_t delay) {
        return eventGroupMockImpl<T, N>.waitBits(message, delay);
    }

    inline static uint32_t MAX_DELAY { 1000 };
};

struct DummyGroup {};
struct DummyMember {};
static_assert(EventGroupConcept<EventGroupMOCK<EventGroupHandle_t, EventBits_t>>);

class StaticTaskMOCK {
public:
    MOCK_METHOD((bool), waitForNotify, (const uint32_t message, const uint32_t delay));
    MOCK_METHOD((void), waitMS, (const uint32_t ms));
    MOCK_METHOD((bool), stopRequested, ());
    MOCK_METHOD((void), yield, ());
};
inline StaticTaskMOCK staticMethodsTaskMOCK;

class TaskMockImpl {
public:
    MOCK_METHOD((bool), notify, (const uint32_t message));
    MOCK_METHOD((bool), requestStop, ());
    MOCK_METHOD((bool), join, ());
    MOCK_METHOD((bool), start, ());
    MOCK_METHOD((bool), isRunning, ());
};
inline TaskMockImpl taskMockImpl;

class TaskMOCK {
public:
    static etl::optional<TaskMOCK> create(void (*task)(void* arg), void* arg, const uint32_t stackSize, const uint32_t priority) {
        return TaskMOCK{};
    }

    bool notify(const uint32_t message) { return taskMockImpl.notify(message); }
    bool requestStop() { return taskMockImpl.requestStop(); }
    bool join() { return taskMockImpl.join(); }
    bool start() { return taskMockImpl.start(); }
    bool isRunning() { return taskMockImpl.isRunning(); }

    static void yield() { staticMethodsTaskMOCK.yield(); }
    static bool waitForNotify(const uint32_t message, const uint32_t delay) { return staticMethodsTaskMOCK.waitForNotify(message, delay); }
    static bool stopRequested() { return staticMethodsTaskMOCK.stopRequested(); }
    static void waitMS(const uint32_t ms) { staticMethodsTaskMOCK.waitMS(ms); }

    inline static uint32_t MAX_DELAY { 10000 };
};
static_assert(TaskConcept<TaskMOCK>);

class SensorMOCK {
public:
    MOCK_METHOD((bool), turnOn, ());
    MOCK_METHOD((bool), turnOff, ());
    MOCK_METHOD((uint32_t), getSample, (), (const));
    MOCK_METHOD((bool), stopListeningIT, ());
    MOCK_METHOD((bool), listenIT, ());
};
static_assert(SensorConcept<SensorMOCK>, "SensorMOCK doesn't meet concept requirments \n");

class PlannerMockImpl {
public:
    MOCK_METHOD((motionBlock_t), calculateFrequency, (const moveBlock_t& move));
    MOCK_METHOD((motionBlock_t), recieve, ());
    MOCK_METHOD((bool), stop, ());
    MOCK_METHOD((bool), start, ());
    MOCK_METHOD((void), enqueue, (const moveBlock_t& move));
};
inline PlannerMockImpl plannerMockImpl;

class PlannerMOCK {
public:
    static etl::optional<PlannerMOCK> create() {
        return PlannerMOCK{};
    }

    motionBlock_t calculateFrequency(const moveBlock_t& move) { return plannerMockImpl.calculateFrequency(move); }
    motionBlock_t recieve() { return plannerMockImpl.recieve(); }
    bool stop() { return plannerMockImpl.stop(); }
    bool start() { return plannerMockImpl.start(); }
    void enqueue(const moveBlock_t& move) { plannerMockImpl.enqueue(move); }
};
static_assert(PlannerConcept<PlannerMOCK>, "PlannerMOCK doesn't meet concept requirments \n");

class StepperMockImpl {
public:
    MOCK_METHOD((void), enqueue, (const motionBlock_t motion));
    MOCK_METHOD((bool), stop, ());
    MOCK_METHOD((bool), start, ());
};
inline StepperMockImpl stepperMockImpl;

class StepperMOCK {
public:
    static etl::optional<StepperMOCK> create(const uint16_t step, const uint16_t dir) {
        return StepperMOCK{};
    }

    void enqueue(const motionBlock_t motion) { stepperMockImpl.enqueue(motion); }
    bool stop() { return stepperMockImpl.stop(); }
    bool start() { return stepperMockImpl.start(); }
};
static_assert(StepperConcept<StepperMOCK>, "StepperMOCK doesn't meet concept requirments \n");

class EngMockImpl {
public:
    MOCK_METHOD((bool), isRunning, (), (const));
    MOCK_METHOD((long), position, ());
    MOCK_METHOD((bool), start, ());
    MOCK_METHOD((bool), stop, ());
    MOCK_METHOD((void), moveImpl, (const moveBlock_t& move, const moveInfo_t flags, const int wait));
    MOCK_METHOD((void), moveToImpl, (const moveBlock_t& move, const moveInfo_t flags, const int wait));
    MOCK_METHOD((void), moveToCupImpl, (const moveBlock_t& move, const moveInfo_t flags, const int wait));
};
inline EngMockImpl engMockImpl;

template <class T, class N, class M>
class EngMOCK {
public:
    using Planner = T;
    using Stepper = N;
    using Task = M;

    static etl::optional<EngMOCK> create(const int8_t stepPin, const int8_t dirPin, Planner& planner, Stepper& engine, Task& taskSpace) {
        return EngMOCK{};
    }

    static void dataRelayTask(void* arg) {}

    bool isRunning() const { return engMockImpl.isRunning(); }
    long position() { return engMockImpl.position(); }
    bool start() { return engMockImpl.start(); }
    bool stop() { return engMockImpl.stop(); }

    void move(const moveBlock_t& move, const moveInfo_t flags = {}, const int wait = 0) { moveImpl(move, flags, wait); }
    void moveImpl(const moveBlock_t& move, const moveInfo_t flags, const int wait) { engMockImpl.moveImpl(move, flags, wait); }

    void moveTo(const moveBlock_t& move, const moveInfo_t flags = {}, const int wait = 0) { moveToImpl(move, flags, wait); }
    void moveToImpl(const moveBlock_t& move, const moveInfo_t flags, const int wait) { engMockImpl.moveToImpl(move, flags, wait); }

    void moveToCup(const moveBlock_t& move, const moveInfo_t flags = {}, const int wait = 0) { moveToCupImpl(move, flags, wait); }
    void moveToCupImpl(const moveBlock_t& move, const moveInfo_t flags, const int wait) { engMockImpl.moveToCupImpl(move, flags, wait); }
};
static_assert(EngineConcept<EngMOCK<PlannerMOCK, StepperMOCK, TaskMOCK>>, "EngMOCK does not meet concept requirments\n");
