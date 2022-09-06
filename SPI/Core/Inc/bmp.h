/*
 * bmp.h
 *
 *  Created on: 19 feb. 2022
 *      Author: Lenovo
 */

#ifndef INC_BMP_H_
#define INC_BMP_H_

#include <stdint.h>


 typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
} tImage;

extern const tImage pajaro;

#endif /* INC_BMP_H_ */
