#if !defined(_stdafx_h)
#define _stdafx_h

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <Shlwapi.h>

#define UN_Alloc(bcLength) malloc(bcLength)
#define UN_Free(p) free((void*)p)
typedef uint32_t UN_ERROR;
#define UN_ERROR_SUCCESS (UN_ERROR)0
#define UN_ERROR_INSUFFICIENT_BUFFER (UN_ERROR)ERROR_INSUFFICIENT_BUFFER
#define UN_ERROR_NOT_ENOUGH_MEMORY	(UN_ERROR)ERROR_NOT_ENOUGH_MEMORY
#define UN_ERROR_INVALID_OPERATION	(UN_ERROR)ERROR_INVALID_OPERATION
#endif

