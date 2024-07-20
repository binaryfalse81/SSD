#pragma once

typedef void            VOID;
typedef char            CHAR;
typedef unsigned char   UINT8;
typedef int             INT32;
typedef unsigned int    UINT32;

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <map>
#include <unordered_map>

#define MAX_LPN                 (100)
#define MAX_ERASE_LPN           (10)
#define MAX_PATTERN_LENGTH      (10)
#define MAX_CMD_BUF_CNT         (10)
#define INITIAL_UPDATE_SIZE     (1)

using namespace std;

extern string UNMAPED_PATTERN;

typedef struct
{
    UINT32 nLpn;
    string strPattern;
    UINT32 nSize;
} NAND_DATA;
