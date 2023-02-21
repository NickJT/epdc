#pragma once

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define dbg(x) std::cout << x
#else
#define dbg(x)
#endif