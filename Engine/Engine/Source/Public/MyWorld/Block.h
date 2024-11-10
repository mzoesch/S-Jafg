#pragma once

struct Block
{
public:
    char topMinX, topMinY, topMaxX, topMaxY;
    char bottomMinX, bottomMinY, bottomMaxX, bottomMaxY;
    char sideMinX, sideMinY, sideMaxX, sideMaxY;

Block(char minX, char minY);
    Block(char topMinX, char topMinY,
    char bottomMinX, char bottomMinY,
    char sideMinX, char sideMinY);
};
