
#pragma once
class Nema;
class ScurvePlanner;
class FastAccelWrapper;
class FreeRtosWrapper;


using EngineType  = Nema;
using PlannerType = ScurvePlanner;
using StepperType = FastAccelWrapper;
using TaskType    = FreeRtosWrapper;