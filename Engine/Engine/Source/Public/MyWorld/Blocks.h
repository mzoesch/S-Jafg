#pragma once

#include <vector>

#include "Block.h"

namespace Blocks
{

const std::vector<Block> blocks{
    Block(0, 0), // Null block
    Block(0, 0), // Air block
    Block(0, 0), // Dirt block

    Block(1, 1,  // Grass block
        0, 0,
        1, 0),

    Block(0, 1)  // Stone block
};

enum BLOCKS
{
    Null = 0,
    AIR = 1,
    DIRT_BLOCK = 2,
    GRASS_BLOCK = 3,
    STONE_BLOCK = 4
};

}
