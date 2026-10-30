
#include "ScurvePlanner.hpp"

template< class InMove, class Task >
void ScurvePlanner<InMove, Task>::scurveTask(void *arg)
{
    auto* instance = static_cast<ScurvePlanner*>(arg);
    auto& movesQ = instance->movesQ;
    auto& destinationEnqueue = instance->destinationEnqueue;
    
    while (!Task::stopRequested()) {
        if (Task::waitForNotify(ScurvePlanner::pauseBit, 0)) {
            while (Task::waitForNotify(ScurvePlanner::resumeBit, 0)) {
                Task::yield();
            } 
        }
        if (!movesQ.empty()) {
            instance->calculateFrequency(movesQ.front(), destinationEnqueue);
            movesQ.pop();
        } else {
            Task::yield();
        }
    }
}

template< class InMove, class Task >
ScurvePlanner<InMove, Task>::ScurvePlanner(Task &task, void (*pDestinationEnqueue)(const OutMove &move))
    : destinationEnqueue(pDestinationEnqueue), 
      taskHandle(task) 
{}

template< class InMove, class Task >
etl::optional<ScurvePlanner<InMove, Task>> ScurvePlanner<InMove, Task>::create(Task &task, void (*pDestinationEnqueue)(const OutMove &move))
{
    ScurvePlanner tmp{task, pDestinationEnqueue};
    return tmp;
}

template< class InMove, class Task >
void ScurvePlanner<InMove, Task>::calculateFrequency(const InMove &move, void (*pEnqueue)(const OutMove &move))
{
    static Scurve calc{}; 
    const auto destinationEnqueue = (pEnqueue) ? pEnqueue : ScurvePlanner::destinationEnqueue;  

    double time { 0 };
    OutMove res;
    
    do{
        auto res = calc.motion(
            move.traversalSpeed,
            move.maxAcc,       
            move.startSpeed,    
            move.startAcc,      
            move.steps,         
            move.endSpeed,      
            move.endAcc,        
            time                
        );

        time += 0.1;
        destinationEnqueue(res);
    }while ( time < res.ct ) ;

}

template< class InMove, class Task >
bool ScurvePlanner<InMove, Task>::stop()
{
    if (!taskHandle.isRunning()) return true;
    if (!taskHandle.join()) return false;
    return true;
}

template< class InMove, class Task >
bool ScurvePlanner<InMove, Task>::start(void (*enqueueAdapter)(const OutMove &move))
{
    if (taskHandle.isRunning()) return false;

    destinationEnqueue = enqueueAdapter; 

    if (!taskHandle.start()) {
        destinationEnqueue = nullptr;
        return false;
    } 
    return true; 
}

template< class InMove, class Task >
bool ScurvePlanner<InMove, Task>::pause()
{
    if (!taskHandle.notify(pauseBit)) return false;
    return true;
}

template< class InMove, class Task >
bool ScurvePlanner<InMove, Task>::resume()
{
    if (!taskHandle.notify(resumeBit)) return false;
    return true;
}

template< class InMove, class Task >
bool ScurvePlanner<InMove, Task>::enqueue(const InMove &motion)
{
    if (movesQ.full()) return false;

    movesQ.push(motion);
    return true;
} 