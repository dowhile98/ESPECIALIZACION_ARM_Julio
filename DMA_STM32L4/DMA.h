/**
  * @autor QUINO B. JEFFRY
	* @version v.1
	*/
	
#ifndef __DMA_H
#define __DMA_H

/*includes*/
#include "stm32l4xx.h"                  // Device header

/*macro defines*/
/***CHANNEL PRIORITIES*****************/
#define CHANNEL_PL_LOW				0x0U<<12
#define CHANNEL_PL_MEDIUM			0x1U<<12
#define CHANNEL_PL_HIGH				0x2U<<12
#define CHANNEL_PL_VERY_HIGH	0x3U<<12


/******CHANNEL PSIZE*******************/
#define CHANNEL_PSIZE_8B			0x0U<<8
#define CHANNEL_PSIZE_16B			0x1U<<8
#define CHANNEL_PSIZE_32B			0x2U<<8


/******CHANNEL MSIZE*******************/
#define CHANNEL_MSIZE_8B			0x0U<<10
#define CHANNEL_MSIZE_16B			0x1U<<10
#define CHANNEL_MSIZE_32B			0x2U<<10


/********CHANNEL DIRECTION*************/
#define CHANNEL_MEM_TO_PERIPH	0x1U<<4
#define CHANNEL_PERIPH_TO_MEM	0x0U<<4


/**
  * @brief configura las direcciones y la cantidad de datos a transmitir
	* @brief DMAx: DMA1/DMA2 que se desea configurar
	* @param DMA_Channelx: canal que se desea configurar
	* @param periphAdd: direccion de la memoria periferica
	* @param memAdd: Direccion de la memoria
	* @param sizse: cantidad de datos a transmitir
	*/
void DMA_Channel_Init(DMA_TypeDef * DMAx,DMA_Channel_TypeDef * DMA_Channelx,uint32_t periphAdd,uint32_t memAdd, uint32_t size);


/**
  * @brief Configura los parametros del registro 
	* @param DMA_Channelx: canal que se desea configurar
	* @param PL: Prioridad del canal
	* @param DIR: direccion de la comunicacion (periferico a memoria/memoria a periferico) 0 en resto de los casos
  * @param CIRC: si se desea el modo circular para el canal
	* @param PINC: si se desea incrementar la direccion del periferico
	* @param MINC: si se desea incrementar la direccion de la memoria
	* @param PSIZE: Tamaño de datos del periférico
	* @param MSIZE: Tamaño de datos de la memoria
	* @param 
	*/
	
void DMA_Channel_Setting(DMA_Channel_TypeDef * DMA_Channelx, uint32_t PL, uint32_t DIR, uint32_t CIRC,uint32_t PINC, uint32_t MINC, uint32_t PSIZE, uint32_t MSIZE);


/**
  * @brief habilita el Canal DMA
  * @param DMA_Channelx: Canal DMA que se quiere habilitar
  */
void DMA_Channel_Enable(DMA_Channel_TypeDef * DMA_Channelx);


/**
  * @brief deshabilita el Canal DMA
  * @param DMA_Channelx: Canal DMA que se quiere habilitar
  */
void DMA_Channel_Disable(DMA_Channel_TypeDef * DMA_Channelx);

#endif /*__DMA_H*/

