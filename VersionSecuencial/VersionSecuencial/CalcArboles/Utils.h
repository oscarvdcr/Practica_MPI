//
// Created by Fernando Cores Prado on 12/4/24.
//

#ifndef CALCCERCA_UTILS_H
#define CALCCERCA_UTILS_H

// Constantes:
#define DDebug false
#define DShowAllCombinations false

#include <limits.h>

// Check windows architecture
#if _WIN32 || _WIN64
#if _WIN64
        #define ENVIRONMENT64
    #else
        #define ENVIRONMENT32
    #endif
#endif

// Check GCC architecture
#if __GNUC__
#if __x86_64__ || __ppc64__ || __arm64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT64
// Constantes
#define DMaxArboles 64

// Combinacicón árboles
typedef unsigned long TCombinacionArboles, *PtrCombinacionArboles;
#else // 32 bits architecture
// Constantes
    #define DMaxArboles 32

    // Combinacicón árboles
    typedef unsigned int TCombinacionArboles, *PtrCombinacionArboles;
#endif

void Error(char *msg);

#endif //CALCCERCA_UTILS_H
