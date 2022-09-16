/*
 * MPU6050.c
 *
 *  Created on: 9 abr. 2022
 *      Author: Lenovo
 */


#include "MPU6050.h"

/* Default I2C address */
#define MPU6050_I2C_ADDR			0x68

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/* Gyro sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

/* Acce sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

static void I2C_Read(uint8_t slaveAddr, uint8_t ADDR, uint8_t *i2cBif, uint8_t NofData);
static void I2C_Write8(uint8_t slaveAddr, uint8_t ADDR, uint8_t data);

extern void I2C_MasterSendData(I2C_TypeDef *I2Cx, uint8_t SlaveAddr,uint8_t *pTxbuffer, uint32_t Len);
extern void I2C_MasterReceiveData(I2C_TypeDef *I2Cx,uint8_t SlaveAddr,uint8_t *pRxbuffer, uint32_t Len);

MPU6050_Result_t MPU6050_Init(MPU6050_t* DataStruct, MPU6050_Device_t DeviceNumber, MPU6050_Accelerometer_t AccelerometerSensitivity, MPU6050_Gyroscope_t GyroscopeSensitivity) {
	uint8_t temp;

	/* Format I2C address */
	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;

	/* Initialize I2C */

	/* Check if device is connected */
//	if (TM_I2C_IsDeviceConnected(MPU6050_I2C, DataStruct->Address) != TM_I2C_Result_Ok) {
//		/* Return error */
//		return TM_MPU6050_Result_DeviceNotConnected;
//	}

	/* Check who am I */
	I2C_Read(DataStruct->Address, MPU6050_WHO_AM_I, &temp, 1);
	if (temp != MPU6050_I_AM) {
		/* Return error */
		return MPU6050_Result_DeviceInvalid;
	}

	/* Wakeup MPU6050 */
	I2C_Write8(DataStruct->Address, MPU6050_PWR_MGMT_1, 0x00);


	/* Set sample rate to 1kHz */
	MPU6050_SetDataRate(DataStruct, MPU6050_DataRate_1KHz);

	/* Config accelerometer */
	MPU6050_SetAccelerometer(DataStruct, AccelerometerSensitivity);

	/* Config accelerometer */
	MPU6050_SetGyroscope(DataStruct, GyroscopeSensitivity);

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_SetGyroscope(MPU6050_t* DataStruct, MPU6050_Gyroscope_t GyroscopeSensitivity) {
	uint8_t temp;

	/* Config gyroscope */
	I2C_Read(DataStruct->Address, MPU6050_GYRO_CONFIG, &temp, 1);

	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;

	I2C_Write8(DataStruct->Address, MPU6050_GYRO_CONFIG, temp);


	switch (GyroscopeSensitivity) {
		case MPU6050_Gyroscope_250s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
			break;
		case MPU6050_Gyroscope_500s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
			break;
		case MPU6050_Gyroscope_1000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
			break;
		case MPU6050_Gyroscope_2000s:
			DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
		default:
			break;
	}

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_SetAccelerometer(MPU6050_t* DataStruct, MPU6050_Accelerometer_t AccelerometerSensitivity) {
	uint8_t temp;

	/* Config accelerometer */
	I2C_Read(DataStruct->Address,  MPU6050_ACCEL_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;

	I2C_Write8(DataStruct->Address, MPU6050_ACCEL_CONFIG, temp);


	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case MPU6050_Accelerometer_2G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
			break;
		case MPU6050_Accelerometer_4G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
			break;
		case MPU6050_Accelerometer_8G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
			break;
		case MPU6050_Accelerometer_16G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
		default:
			break;
	}

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_SetDataRate(MPU6050_t* DataStruct, uint8_t rate) {
	/* Set data sample rate */

	I2C_Write8(DataStruct->Address, MPU6050_SMPLRT_DIV, rate);
//	if (TM_I2C_Write(MPU6050_I2C, DataStruct->Address, MPU6050_SMPLRT_DIV, rate) != TM_I2C_Result_Ok) {
//		/* Return error */
//		return TM_MPU6050_Result_Error;
//	}

	/* Return OK */
	return MPU6050_Result_Ok;
}


MPU6050_Result_t MPU6050_EnableInterrupts(MPU6050_t* DataStruct) {
	uint8_t temp;

	/* Enable interrupts for data ready and motion detect */
	I2C_Write8(DataStruct->Address, MPU6050_INT_ENABLE, 0x21);


	/* Clear IRQ flag on any read operation */
	I2C_Read(DataStruct->Address, MPU6050_INT_PIN_CFG, &temp, 1);

	temp |= 0x10;
	I2C_Write8(DataStruct->Address, MPU6050_INT_PIN_CFG, temp);

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_DisableInterrupts(MPU6050_t* DataStruct) {
	/* Disable interrupts */
	I2C_Write8(DataStruct->Address, MPU6050_INT_ENABLE, 0x00);
//	if (TM_I2C_Write(MPU6050_I2C, DataStruct->Address, MPU6050_INT_ENABLE, 0x00) != TM_I2C_Result_Ok) {
//		/* Return error */
//		return TM_MPU6050_Result_Error;
//	}

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_ReadInterrupts(MPU6050_t* DataStruct, MPU6050_Interrupt_t* InterruptsStruct) {
	uint8_t read;

	/* Reset structure */
	InterruptsStruct->Status = 0;

	/* Read interrupts status register */
	I2C_Read(DataStruct->Address, MPU6050_INT_STATUS, &read, 1);


	/* Fill value */
	InterruptsStruct->Status = read;

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_ReadAccelerometer(MPU6050_t* DataStruct) {
	uint8_t data[6];

	/* Read accelerometer data */
	I2C_Read(DataStruct->Address,MPU6050_ACCEL_XOUT_H, data, 6);

	/* Format */
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_ReadGyroscope(MPU6050_t* DataStruct) {
	uint8_t data[6];

	/* Read gyroscope data */
	I2C_Read(DataStruct->Address,MPU6050_GYRO_XOUT_H, data, 6);

	/* Format */
	DataStruct->Gyroscope_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Gyroscope_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Gyroscope_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_ReadTemperature(MPU6050_t* DataStruct) {
	uint8_t data[2];
	int16_t temp;

	/* Read temperature */
	I2C_Read(DataStruct->Address,MPU6050_TEMP_OUT_H, data, 2);

	/* Format temperature */
	temp = (data[0] << 8 | data[1]);
	DataStruct->Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);

	/* Return OK */
	return MPU6050_Result_Ok;
}

MPU6050_Result_t MPU6050_ReadAll(MPU6050_t* DataStruct) {
	uint8_t data[14];
	int16_t temp;

	/* Read full raw data, 14bytes */
	I2C_Read(DataStruct->Address,MPU6050_ACCEL_XOUT_H, data, 14);

	/* Format accelerometer data */
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);

	/* Format gyroscope data */
	DataStruct->Gyroscope_X = (int16_t)(data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Y = (int16_t)(data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Z = (int16_t)(data[12] << 8 | data[13]);

	/* Return OK */
	return MPU6050_Result_Ok;
}

//2- i2c Read
void I2C_Read(uint8_t slaveAddr, uint8_t ADDR, uint8_t *i2cBif, uint8_t NofData)
{
	I2C_MasterSendData(MPU6050_I2C, slaveAddr, &ADDR, 1);

	I2C_MasterReceiveData(MPU6050_I2C, slaveAddr,i2cBif,NofData);
}

//3- i2c Write
void I2C_Write8(uint8_t slaveAddr, uint8_t ADDR, uint8_t data)
{
	uint8_t i2cData[2];
	i2cData[0] = ADDR;
	i2cData[1] = data;

	I2C_MasterSendData(MPU6050_I2C, slaveAddr, i2cData, 2);

}
