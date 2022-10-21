#pragma once

#include "EngineInterface/GpuSettings.h"

#include "Base.cuh"
#include "Definitions.cuh"
#include "RawMemory.cuh"

struct Objects
{
    Array<Cell*> cellPointers;
    Array<Particle*> particlePointers;

    Array<Cell> cells;
    Array<Particle> particles;

    Array<uint8_t> additionalData;

    void init();
    void free();

    __device__ void saveNumEntries();
};

