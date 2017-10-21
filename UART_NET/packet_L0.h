#if !defined(_packet_L0_h)
#define _packet_L0_h
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _SPacket_L0 {
	SFSM base;
	uint8_t *DT; // data
	uint8_t bcData; // data byte counter
	uint8_t HM; // header mark
	uint8_t DA, dA; // major and minor destination address
	uint8_t SA, sA; // major and minor source address
	uint8_t DL; // data length
	uint8_t ID; // packet ID
	uint16_t CRC; // CRC
} SPacket_L0, *PSPacket_L0;

#if defined(__cplusplus)
}
#endif
#endif