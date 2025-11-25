#include "inc/crc16_modbus.h"

unsigned short crc16_modbus::CRC16(unsigned char* pucFrame, unsigned short usLen)
{
	unsigned char ucCRCHi = 0xFF;
	unsigned char  ucCRCLo = 0xFF;
	int		iIndex;

	while (usLen--)
	{
		iIndex = ucCRCLo ^ *(pucFrame++);
		ucCRCLo = (unsigned char)(ucCRCHi ^ CRCHi[iIndex]);
		ucCRCHi = CRCLo[iIndex];
	}
	return (unsigned short)(ucCRCHi << 8 | ucCRCLo);
}
