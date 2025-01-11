////////////////////////////////////////////////////////////////////////////////
// Buffers.h -- Buffers class include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

// 12/24/24: ONLY NEED 1 call, pass in sizes of attributes
void setupBuffers(
    unsigned int& VAO,
    unsigned int& VBO,
    unsigned int& EBO,
    const float* vertices,
    size_t verticesSize,
    const unsigned int* indices,
    size_t indicesSize,
    const std::vector<int>& attributeSizes
);
