#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"  
#include "i2c.h"


#define BMP280_I2C_ADDRESS 	0x76  	// BMP280 I2C address
#define BMP280_CALIB_START 	0x88 		// Calibration data start address
#define BMP280_CTRL_MEAS    0xF4 		// Control measurement register
#define BMP280_TEMP_DATA    0xFA 		// Temperature data start address

int16_t dig_T1, dig_T2, dig_T3;
char buffer[100];

void BMP280_ReadCalibrationData() {
    uint8_t calib_data[6];

    calib_data[0] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START);
    calib_data[1] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START + 1);
    calib_data[2] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START + 2);
    calib_data[3] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START + 3);
    calib_data[4] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START + 4);
    calib_data[5] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_CALIB_START + 5);

    dig_T1 = (uint16_t)((calib_data[1] << 8) | calib_data[0]);
    dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);
}

int32_t BMP280_ReadRawTemperature() {
    uint8_t temp_data[3];
	
		temp_data[0] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_TEMP_DATA);
		temp_data[1] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_TEMP_DATA + 1);
		temp_data[2] = I2C_ReadByte(BMP280_I2C_ADDRESS, BMP280_TEMP_DATA + 2);
	
    int32_t raw_temp = ((int32_t)temp_data[0] << 12) |
                       ((int32_t)temp_data[1] << 4) |
                       ((temp_data[2] >> 4) & 0x0F);
	
		return raw_temp;
		
}

float BMP280_ConvertToCelsius(int32_t raw_temperature) {
    int32_t var1, var2, t_fine;
    float temperature;

    var1 = ((((raw_temperature >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((raw_temperature >> 4) - ((int32_t)dig_T1)) *
              ((raw_temperature >> 4) - ((int32_t)dig_T1))) >> 12) *
            ((int32_t)dig_T3)) >>
           14;
    t_fine = var1 + var2;

    temperature = (t_fine * 5 + 128) >> 8;  
    return temperature / 100;             
}

float BMP280_ReadTemperature() {
    float temperature_sum = 0.0f;
    float temperature_avg = 0.0f;

    for (int i = 0; i < 128; i++) {
        int32_t raw_temp = BMP280_ReadRawTemperature();
        float temperature = BMP280_ConvertToCelsius(raw_temp);
        temperature_sum += temperature;
    }

    temperature_avg = temperature_sum / 128.0f;
		return temperature_avg;
}

void BMP280_Init() {
    I2C_WriteByte(BMP280_I2C_ADDRESS, BMP280_CTRL_MEAS, 0x23); // Enable temperature measurement, normal mode
    BMP280_ReadCalibrationData();
}


/*

int main() {

		I2C_Init();
		BMP280_Init();
		for (int i = 0; i < 1000000; i++){}
			
    while (1) {
			
			BMP280_ReadTemperature();
			for (int i = 0; i < 1000000; i++){}
			

    }

}


*/
