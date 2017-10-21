#include "stdafx.h"
#include "helper.h"
#include "crc16.h"

void CRC16_Put(PSCRCRegisters crc, uint8_t u8Data)
{
	for (int i = 0; i < 8; i++)
	{
		bool msbIn = ((u8Data & 1) ^ (uint8_t)(crc->SR & 1)) != 0;
		crc->SR =
			(msbIn ? (1 << 15) : 0) |
			(crc->SR >> 1) ^ (msbIn ? crc->FP : 0);
		u8Data = u8Data >> 1;
	}
}

void CRC16_Puts(PSCRCRegisters crc, const uint8_t * pu8Data, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		CRC16_Put(crc, pu8Data[i]);
	}
}

void CRC16_Puts2(PSCRCRegisters crc, PSSizedBuffer dataBuffer)
{
	CRC16_Puts(crc, dataBuffer->buffer, dataBuffer->bcLength);
}
