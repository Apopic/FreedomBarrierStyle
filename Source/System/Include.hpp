#pragma once

/*
*  using
*/
using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ulonglong = unsigned long long;

/*
*  define
*/
#define PI (3.1415926535897932384626433832795028841971)

/*
*  STLs
*/
#include <algorithm>
#include <array>
#include <atomic>
#include <ctime>
#include <complex>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <time.h>
#include <queue>
#include <random>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

/*
*  Library
*/
#include "Library/LibraryInclude.h"

/*
*  namespace
*/
namespace fs = std::filesystem;

#define switch_h(val) switch (std::hash<std::string_view>()(val))
#define case_h(val) case val##_murmur3: