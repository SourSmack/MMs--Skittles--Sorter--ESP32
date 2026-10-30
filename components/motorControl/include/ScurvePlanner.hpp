#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <etl/circular_buffer.h>
#include <etl/optional.h>

#include "moveStructures.hpp"
#include "FreeRTOSWrapper.hpp"
#include "s-curve-motion-planning/scurve.h"

template< class InMove, class Task = FREETask >
class ScurvePlanner 
{
private:
    using OutMove = Scurve::RESULT;
    
    enum {
        stopBit   = 0,
        pauseBit  = 1 << 1,
        resumeBit = 1 << 2 
    };
    
    static constexpr int MAXBUFFOR { 64 };
    etl::circular_buffer<InMove, MAXBUFFOR> movesQ{}; 
    
    void (*destinationEnqueue)(const OutMove &move) { nullptr };

    Task &taskHandle;
    
    static void scurveTask(void *arg);

public:
    friend class ScurveTesting;
   
    ScurvePlanner(Task &task, void (*pDestinationEnqueue)(const OutMove &move) = nullptr);

    static etl::optional<ScurvePlanner> create(Task &task, void (*pDestinationEnqueue)(const OutMove &move));

    void calculateFrequency(const InMove &move, void (*pEnqueue)(const OutMove &move) = nullptr);

    bool stop();
    
    bool start(void (*enqueueAdapter)(const OutMove &move));

    bool pause();

    bool resume();

    bool enqueue(const InMove &motion);
};