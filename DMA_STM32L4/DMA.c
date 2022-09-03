/**
  * @autor QUINO B. JEFFRY
	* @version v.1
	*/
	
#include "DMA.h"

/**
  * @brief configura un canal del del DMAx
	* @brief DMAx: DMA1/DMA2 que se desea configurar
	* @param DMA_Channel: canal que se desea configurar
	* @param periphAdd: direccion de la memoria periferica
	* @param memAdd: Direccion de la memoria
	* @param sizse: cantidad de datos a transmitir
	*/
void DMA_Channel_Init(DMA_TypeDef * DMAx,DMA_Channel_TypeDef * DMA_Channelx,uint32_t periphAdd,uint32_t memAdd, uint32_t size){
	/*enable clock*/
	if(DMAx == DMA1)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	else if(DMAx == DMA2)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	else
		while(1);													//ERROR
	/*1. Configure la dirección del registro de periféricos */
	DMA_Channelx->CPAR = periphAdd;
	/*2. Configure la dirección de memoria en el registro DMA_CMARx. */
	DMA_Channelx->CMAR = memAdd;
	/*3. Configure el número total de datos para transferir */
	DMA_Channelx->CNDTR = size;
	
	return;
}

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
	
void DMA_Channel_Setting(DMA_Channel_TypeDef * DMA_Channelx, uint32_t PL, uint32_t DIR, uint32_t CIRC,uint32_t PINC, uint32_t MINC, uint32_t PSIZE, uint32_t MSIZE){
	DMA_Channelx->CCR = 0;															//reset
	/*La prioridad del canal*/
	DMA_Channelx->CCR |= PL;
	
	/*La dirección de transferencia de datos*/
	DMA_Channelx->CCR |= DIR;
	
	/*El modo circular*/
	DMA_Channelx->CCR |= CIRC;
	
	/*El modo de periférico y de memoria incrementada.*/
	DMA_Channelx->CCR |= PINC | MINC;
	
	/*el tamaño de los datos de la memoria y el periférico.*/
	DMA_Channelx->CCR |= PSIZE | MSIZE;
	
	return;
}


/**
  * @brief habilita el Canal DMA
  * @param DMA_Channelx: Canal DMA que se quiere habilitar
  */
void DMA_Channel_Enable(DMA_Channel_TypeDef * DMA_Channelx){
	
	DMA_Channelx->CCR |= DMA_CCR_EN;
	return;
}

/**
  * @brief deshabilita el Canal DMA
  * @param DMA_Channelx: Canal DMA que se quiere habilitar
  */
void DMA_Channel_Disable(DMA_Channel_TypeDef * DMA_Channelx){
	DMA_Channelx->CCR &=~ DMA_CCR_EN;
	return;
}

