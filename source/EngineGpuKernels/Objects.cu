﻿#include "Objects.cuh"

#include "Cell.cuh"
#include "Particle.cuh"

void Objects::init()
{
    cellPointers.init();
    cells.init();
    particles.init();
    particlePointers.init();
    additionalData.init();
}

void Objects::free()
{
    cellPointers.free();
    cells.free();
    particles.free();
    particlePointers.free();
    additionalData.free();
}

__device__ void Objects::saveNumEntries()
{
    cellPointers.saveNumEntries();
    particlePointers.saveNumEntries();
    cells.saveNumEntries();
    particles.saveNumEntries();
}
