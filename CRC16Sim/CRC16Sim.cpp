// CRC16Sim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef struct _SCRCRegisters {
	uint16_t 
		SR, // shift register
		FP; // feedback polynomial
} SCRCRegisters, *PSCRCRegisters;

void Put(PSCRCRegisters crc, uint8_t u8Data);
void Puts(PSCRCRegisters crc, uint8_t* pu8Data, size_t length);
void FillRand(uint8_t* pu8Data, size_t length);

int main()
{
	// create CRC module
	SCRCRegisters crc = {
		0, // shift register
		(1 << 3) | (1 << 10) // feedback polynomial
	};

	// initialize data
	size_t dataLength = 256;
	uint8_t *data = new uint8_t[dataLength];
	FillRand(data, dataLength);
	uint16_t crcOffset = 0;
	crc.SR = crcOffset;
	wprintf(L"CRC0 = 0x%04x\n", crc.SR);

	// calculate CRC
	Puts(&crc, data, dataLength);
	wprintf(L"CRC1 = 0x%04x\n", crc.SR);
	uint16_t SRSave = crc.SR;

	// clear CRC module
	crc.SR = crcOffset;

	// put the same data and CRC
	data[0] ^= 0x40;
	data[0] ^= 0x40;
	Puts(&crc, data, dataLength);
	Puts(&crc, (uint8_t*)&SRSave, 2);
	wprintf(L"CRC2 = 0x%04x\n", crc.SR);
    return 0;
}

void Put(PSCRCRegisters crc, uint8_t u8Data)
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

void Puts(PSCRCRegisters crc, uint8_t * pu8Data, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		Put(crc, pu8Data[i]);
	}
}

void FillRand(uint8_t * pu8Data, size_t length)
{
	time_t t = time(nullptr);
	srand((unsigned int)t);
	for (size_t i = 0; i < length; i++)
	{
		pu8Data[i] = (uint8_t)(rand() & 0xff);
	}
}
