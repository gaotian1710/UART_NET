#if !defined(_helper_h)
#define _helper_h

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _SSizedBuffer {
	uint8_t *buffer;
	size_t  bcLength;
} SSizedBuffer, *PSSizedBuffer;

/*!
\brief create a new buffer
\param bcLength [in] buffer length counted in bytes
\return pointer to a new buffer; NULL is returned if not enough memory available.
*/
PSSizedBuffer SizedBuffer_New(size_t bcLength);

/*!
\brief delete an existing buffer
\param p [in] pointer to the buffer pointer
*/
void SizedBuffer_Delete(PSSizedBuffer *p);

/*!
\brief fill buffer with random numbers
\param p [in] uint8_t buffer with length; length must be given
*/
void SizedBuffer_FillRand(PSSizedBuffer p);

typedef struct _SLogger {
	SSizedBuffer coreBuffer;
	SSizedBuffer exportBuffer;
	SSizedBuffer filePath;
	uint32_t code; // error code, etc.
	CRITICAL_SECTION cs;
} SLogger, *PSLogger;

/*!
\brief create a new Logger instance
\param bcLength [in] buffer length of coreBuffer
\param filename [in] filename of the log file without directory path
	which is given by ModuleFileName(). <exename>.log is used when
	filename is NULL.
\return pointer to a Logger instance
*/
PSLogger Logger_New(size_t bcLength, const char* filename);

/*!
\brief delete an existing buffer 
\param p [in] pointer to the buffer pointer
*/
void Logger_Delete(PSLogger *p);

/*!
\brief fill head of coreBuffer with time and code and export the remaining
	part of the buffer.
\param _code [in] a number put in p->code
\return pointer to p->exportBuffer which is filled with the remaining part of
	p->coreBuffer. Head part of coreBuffer is filled with time stamp(ISO format)
	and code(%08x) delimited with ','.
*/
PSSizedBuffer Logger_ExportBuffer(PSLogger p, uint32_t _code);

/*!
\brief release the logger and flush coreBuffer to the log file.
\param p [in] a pointer to a logger instance.
\return error code representing errors during writing file operation.
*/
uint32_t Logger_Flush(PSLogger p);

void Logger_Info(PSLogger p, const uint8_t* infoString);

void Logger_Code(PSLogger p, uint32_t _code, const uint8_t* infoString);

#define ARRAY_DIM(a) (sizeof(a)/sizeof(a[0]))
#if defined(__cplusplus)
}
#endif
#endif
