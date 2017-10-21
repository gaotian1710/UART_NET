#if !defined(_crc16_h)
#define _crc16_h
#if defined(__cplusplus)
extern "C" {
#endif 

#define DEFAULT_CRC16FP ((1 << 3) | (1 << 10))
typedef struct _SCRCRegisters {
	uint16_t
		SR, // shift register
		FP; // feedback polynomial
} SCRCRegisters, *PSCRCRegisters;

void CRC16_Put(PSCRCRegisters crc, uint8_t u8Data);
void CRC16_Puts(PSCRCRegisters crc, const uint8_t* pu8Data, size_t length);
void CRC16_Puts2(PSCRCRegisters crc, PSSizedBuffer dataBuffer);

#if defined(__cplusplus)
}
#endif 
#endif
