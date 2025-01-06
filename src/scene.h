#pragma once

#include <vector>

std::vector<float> floorVertices = {
    // Positions            // Normals        // Texture coordinates
    -50.0f, -15.0f, -50.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
     50.0f, -15.0f, -50.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
     50.0f, -15.0f,  50.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
    -50.0f, -15.0f,  50.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f
};
std::vector<unsigned int> floorIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> frontWallVertices = {
    -50.0f, -15.0f, -50.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
     50.0f, -15.0f, -50.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
     50.0f, 40.0f, -50.0f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
    -50.0f, 40.0f, -50.0f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f
};
std::vector<unsigned int> frontWallIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> leftWallVertices = {
    -50.0f, -15.0f,  50.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    -50.0f, -15.0f, -50.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    -50.0f, 40.0f, -50.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
    -50.0f, 40.0f,  50.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f
};

std::vector<unsigned int> leftWallIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> rightWallVertices = {
     50.0f, -15.0f, -50.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
     50.0f, -15.0f,  50.0f,    -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
     50.0f, 40.0f,  50.0f,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
     50.0f, 40.0f, -50.0f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f
};

std::vector<unsigned int> rightWallIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> backWallVertices = {
     50.0f, -15.0f,  50.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
    -50.0f, -15.0f,  50.0f,    0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
    -50.0f, 40.0f,  50.0f,   0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
     50.0f, 40.0f,  50.0f,   0.0f, 0.0f, -1.0f,    0.0f, 1.0f
};

std::vector<unsigned int> backWallIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> ceilingVertices = {
    -50.0f, 40.0f, -50.0f,    0.0f, -1.0f, 0.0f,    0.0f, 0.0f,
     50.0f, 40.0f, -50.0f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f,
     50.0f, 40.0f,  50.0f,    0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
    -50.0f, 40.0f,  50.0f,    0.0f, -1.0f, 0.0f,    0.0f, 1.0f
};

std::vector<unsigned int> ceilingIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<float> pedestalVertices = {
    -15.0f, -6.8f,  -15.0f,   0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
     15.0f, -6.8f,  -15.0f,   0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
     15.0f, -6.8f,   15.0f,   0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
    -15.0f, -6.8f,   15.0f,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
    -15.0f, -6.8f,  -15.0f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
     15.0f, -6.8f,  -15.0f,   0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
     15.0f, -15.0f, -15.0f,   0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
    -15.0f, -15.0f, -15.0f,   0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
    -15.0f, -6.8f,   15.0f,   1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
    -15.0f, -6.8f,  -15.0f,   1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
    -15.0f, -15.0f, -15.0f,   1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
    -15.0f, -15.0f,  15.0f,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
     15.0f, -6.8f,  -15.0f,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
     15.0f, -6.8f,   15.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
     15.0f, -15.0f,  15.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
     15.0f, -15.0f, -15.0f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
     15.0f, -6.8f,   15.0f,   0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
    -15.0f, -6.8f,   15.0f,   0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
    -15.0f, -15.0f,  15.0f,   0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
     15.0f, -15.0f,  15.0f,   0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
    -15.0f, -15.0f, -15.0f,   0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
     15.0f, -15.0f, -15.0f,   0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
     15.0f, -15.0f,  15.0f,   0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
    -15.0f, -15.0f,  15.0f,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f
};

std::vector<unsigned int> pedestalIndices = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    8, 9, 10,
    10, 11, 8,
    12, 13, 14,
    14, 15, 12,
    16, 17, 18,
    18, 19, 16,
    20, 21, 22,
    22, 23, 20
};
