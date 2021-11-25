#pragma once

struct Cell;
struct Token;
struct Particle;
struct Entities;

struct SimulationData;
class SimulationResult;
class SelectionResult;
struct CellAccessTO;
struct ClusterAccessTO;
struct DataAccessTO;
struct SimulationParameters;
struct GpuSettings;
class CudaMonitorData;

struct ApplyForceData
{
    float2 startPos;
    float2 endPos;
    float2 force;
    float radius;
    bool onlyRotation;
};

struct SwitchSelectionData
{
    float2 pos;
    float radius;
};

struct SetSelectionData
{
    float2 startPos;
    float2 endPos;
};

struct SelectedEntitites
{
    int numCells;
    int numIndirectCells;
    int numParticles;
};

struct MoveSelectionData
{
    float2 displacement;
};


#define FP_PRECISION 0.00001

#define CUDA_THROW_NOT_IMPLEMENTED() printf("not implemented"); \
    asm("trap;");