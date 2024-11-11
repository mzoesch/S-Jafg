#include "CoreAfx.h"
#include "MyWorld/Block.h"

Block::Block(char minX, char minY)
{
    topMinX = minX;
    topMinY = minY;
    topMaxX = static_cast<char>(minX + 1);
    topMaxY = static_cast<char>(minY + 1);

    bottomMinX = minX;
    bottomMinY = minY;
    bottomMaxX = static_cast<char>(minX + 1);
    bottomMaxY = static_cast<char>(minY + 1);

    sideMinX = minX;
    sideMinY = minY;
    sideMaxX = static_cast<char>(minX + 1);
    sideMaxY = static_cast<char>(minY + 1);
}

Block::Block(char topMinX, char topMinY,
    char bottomMinX, char bottomMinY,
    char sideMinX, char sideMinY)
{
    this->topMinX = topMinX;
    this->topMinY = topMinY;
    this->topMaxX = static_cast<char>(topMinX + 1);
    this->topMaxY = static_cast<char>(topMinY + 1);

    this->bottomMinX = bottomMinX;
    this->bottomMinY = bottomMinY;
    this->bottomMaxX = static_cast<char>(bottomMinX + 1);
    this->bottomMaxY = static_cast<char>(bottomMinY + 1);

    this->sideMinX = sideMinX;
    this->sideMinY = sideMinY;
    this->sideMaxX = static_cast<char>(sideMinX + 1);
    this->sideMaxY = static_cast<char>(sideMinY + 1);
}
