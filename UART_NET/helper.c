#include "stdafx.h"
#include "helper.h"

PSSizedBuffer SizedBuffer_New(size_t bcLength)
{
	PSSizedBuffer p = (PSSizedBuffer)UN_Alloc(sizeof(SSizedBuffer));
	p->bcLength = bcLength;
	p->buffer = (uint8_t*)UN_Alloc(bcLength);
	memset((void*)(p->buffer), 0, bcLength);
	return p;
}

void SizedBuffer_Delete(PSSizedBuffer * p)
{
	if (*p)
	{
		if ((*p)->buffer)
		{
			UN_Free((*p)->buffer);
		}
		UN_Free(*p);
		*p = NULL;
	}
}

void SizedBuffer_FillRand(PSSizedBuffer p)
{
	for (size_t i = 0; i < p->bcLength; i++)
	{
		p->buffer[i] = (uint8_t)rand();
	}
}

PSLogger Logger_New(size_t bcLength, const char * filename)
{
	uint8_t *logFilePath = (uint8_t*)UN_Alloc(MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH, logFilePath);
	PathAppendA((LPSTR)logFilePath, filename);

	PSLogger p = (PSLogger)UN_Alloc(sizeof(SLogger));
	p->coreBuffer.bcLength = bcLength;
	p->coreBuffer.buffer = (uint8_t*)UN_Alloc(bcLength);
	p->filePath.bcLength = strlen(logFilePath) + 1;
	p->filePath.buffer = (uint8_t*)UN_Alloc(p->filePath.bcLength);
	strcpy_s(p->filePath.buffer, p->filePath.bcLength, logFilePath);
	UN_Free(logFilePath);

	if (!PathFileExistsA(p->filePath.buffer))
	{
		HANDLE h = CreateFileA(p->filePath.buffer, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(h);
	}

	p->code = UN_ERROR_SUCCESS;
	InitializeCriticalSection(&(p->cs));
	return p;
}

void Logger_Delete(PSLogger * p)
{
	PSLogger pLogger = *p;
	DeleteCriticalSection(&(pLogger->cs));
	UN_Free(pLogger->filePath.buffer);
	UN_Free(pLogger->coreBuffer.buffer);
	UN_Free(pLogger);
	*p = NULL;
}

PSSizedBuffer Logger_ExportBuffer(PSLogger p, uint32_t _code)
{
	static SYSTEMTIME st;
	int writtenLength = 0;

	EnterCriticalSection(&(p->cs));
	p->code = _code;
	GetLocalTime(&st);
	p->exportBuffer = p->coreBuffer;
	writtenLength = snprintf(p->exportBuffer.buffer, p->exportBuffer.bcLength,
		"%02d:%02d:%02d.%03d,0x%08x,",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, p->code);
	p->exportBuffer.buffer += writtenLength;
	p->exportBuffer.bcLength -= writtenLength;
	return &(p->exportBuffer);
}

uint32_t Logger_Flush(PSLogger p)
{
	uint32_t err = UN_ERROR_SUCCESS;
	HANDLE h = INVALID_HANDLE_VALUE;
	do {
		size_t textLength = strlen(p->coreBuffer.buffer);
		DWORD writtenLength = 0;
		if (p->coreBuffer.buffer[textLength - 1] != '\n')
		{
			textLength += 
				snprintf(p->coreBuffer.buffer + textLength, p->coreBuffer.bcLength - textLength,
					"\r\n");
		}

		h = CreateFileA(p->filePath.buffer, FILE_APPEND_DATA, 0,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			err = GetLastError();
			break;
		}

		if (!WriteFile(h, p->coreBuffer.buffer, textLength, &writtenLength, NULL))
		{
			err = GetLastError();
			break;
		}
	} while (false);
	CloseHandle(h);
	LeaveCriticalSection(&(p->cs));
	return err;
}

void Logger_Info(PSLogger p, const uint8_t * infoString)
{
	PSSizedBuffer buffer = Logger_ExportBuffer(p, p->code);
	snprintf(buffer->buffer, buffer->bcLength, "%s\r\n", (LPCSTR)infoString);
	Logger_Flush(p);
}

void Logger_Code(PSLogger p, uint32_t _code, const uint8_t * infoString)
{
	PSSizedBuffer buffer = Logger_ExportBuffer(p, _code);
	snprintf(buffer->buffer, buffer->bcLength, "%s\r\n", (LPCSTR)infoString);
	Logger_Flush(p);
}

