#ifndef __MODBUSMSG_H
#define __MODBUSMSG_H


#include <stdbool.h>

#define MODBUS_ADDR_BROADCAST	(0xFF)
#pragma  pack(1)
typedef struct
{
	unsigned char imuEn;//1:imu on 0:imu off
	unsigned char temperatureEn;//1:temperature on 0:temperature off
	unsigned char ledEn;//1:led on 0:led off
	unsigned short uploadHz;//1~65535
	unsigned char modbusAddr;//1~254
}DeviceConfig_TypeDef;
#pragma  pack()
#pragma pack(1)
	typedef struct MeasureMsg_s
	{
		MeasureMsg_s()
		{
			quat_wxyz[0] = 1.0f;
			quat_wxyz[1] = 0.0f;
			quat_wxyz[2] = 0.0f;
			quat_wxyz[3] = 0.0f;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					fsr[i][j] = 0;
				}
			}
		}
		/*我的地址*/
		unsigned char addr;
		/*功能码*/
		unsigned char funcCode;
		/*
		0xA5
		*/
		unsigned char syncCode1;
		/*
		0x5A
		*/
		unsigned char syncCode2;
		/*0~255循环*/
		unsigned char frameNumber;
		/*
		* 压力片的阵列压力值
		x轴是列 共8列
		y轴是行 共4行

		左下角是坐标原点 x轴向右 y轴向上

			^      ********
			|      ********
			y      ********
			x->  ********
		*/
		unsigned short fsr[4][8];
		/*四元数 wxyz顺序*/
		float quat_wxyz[4];
		/*欧拉角 单位角度 从四元数通过ZXY旋转顺序旋转而来*/
		//float euler_degree[3];
		float temperature;
		unsigned short crc16;
	}MeasureMsg_TypeDef;

	typedef struct
	{
		/*我的地址*/
		unsigned char addr;
		/*功能码*/
		unsigned char funcCode;//0x02
		unsigned char mode;//1:configmode 2:measurement mode
		unsigned short crc16;
	}ModeMsg_TypeDef;

	typedef struct{
		/*我的地址*/
		unsigned char addr;
		/*功能码*/
		unsigned char funcCode;//0x03
		DeviceConfig_TypeDef config;
		unsigned short crc16;
	}SensorOnOffMsg_TypeDef;
#pragma pack()


#endif
