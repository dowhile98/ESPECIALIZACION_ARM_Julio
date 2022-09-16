/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "USART.h"
#include "delay.h"
#include "defines.h"
/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <wizchip_conf.h>
#include <socket.h>
#include <MQTT/MQTTClient.h>
#include <stdio.h>
#include <lwjson.h>
/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/**
 * 0: printf mediante el usart2
 * 1: printf mediante el SWO
 */
#define USE_SWV			0

#define USART2_TX		A, 2
#define USART2_RX		A, 3
#define SPI1_SCK		A, 5
#define SPI1_MOSI		A, 7
#define SPI1_MISO		A, 6
#define SPI1_NSS		B, 6
#define ETH_RST			C, 7
#define LED2			B, 4
/* Private macro -------------------------------------------------------------*/

#define MQTT_SOCKET		0
#define BUFFER_SIZE	2048
/*private struct -------------------------------------------------------------*/
//MQTT
uint8_t targetIP[4] = {192,168,16,101};
//uint8_t targetIP[] = "broker.mqtt-dashboard.com";
uint16_t targetPort = 1883;
uint8_t tempBuffer[BUFFER_SIZE] = {};

struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	enum QoS qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
} opts  = {
  .clientid = (char*)"stdout-subscriber",
  .nodelimiter = 0,
  .delimiter = (char*)"\n",
  .qos = QOS0,
  .username = "xfprwrvc",
  .password = "0ks6T_RP33kF",
  .host = (char*)"localhost",
  .port = 1883,
  .showtopics = 1
};

static lwjson_token_t tokens[128];
static lwjson_t lwjson;

/* Private variables ---------------------------------------------------------*/
uint8_t txbuffer[100];
uint8_t rxbuffer[100];
uint16_t len;


/* Private function prototypes -----------------------------------------------*/
void USART2_GPIOInit(void);

/**
 * @brief configuracion de pines necesarios para el SPI1
 */
void SPI1_GPIOInit(void);

void SPI_Init(SPI_TypeDef *SPIx);
/**
 * @brief SPI send data
 */
void SPI_SendData(SPI_TypeDef *SPIx,uint8_t *pTxBuffer, uint32_t Len);
/**
 * @brief transmitirRecibir
 */
void SPI_SendReceiveData(SPI_TypeDef *SPIx,uint8_t *pTxBuffer,uint8_t *pRxBuffer,uint32_t Len);
/**
 * @brief SPI receive data
 */
void SPI_ReceiveData(SPI_TypeDef *SPIx,uint8_t *pRxBuffer,uint32_t Len);
/**
 * @brief master receive data
 */
void SPI_MasterReceiveData(SPI_TypeDef *SPIx,uint8_t *pRxBuffer,uint32_t Len);

/**
 * @brief genera el reset para el ethernet
 */
void ETH_Reset(void);

void led_blink(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t num_blink, uint32_t delay);

/******************************************************************************/
//chip select
void ETH_CS_Select(void);
void ETH_CS_Unselect(void);
//enviar byte
void ETH_SPI_wb(uint8_t b);
//recibir byte
uint8_t ETH_SPI_rb(void);
/*****************************************************************************/
/**
 * @brief callback  mqtt
 */
void messageArrived(MessageData* md);
/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  uint8_t bufSize[] = {2, 2, 2, 2};
  unsigned char buf[100];
  int rc = 0;
  delay_init_it();
  USART2_GPIOInit();
  USART_Init(USART2, 16E+6, 115200, 0);
  RCC->AHB1ENR |= GPIOX_CLOCK(BUTTON);
  GPIOX_MODER(MODE_DIGITAL_INPUT, BUTTON);
  RCC->AHB1ENR |= GPIOX_CLOCK(ETH_RST);
  GPIOX_MODER(MODE_OUT, ETH_RST);

  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));
/****************************************************************************/
  SPI1_GPIOInit();
  SPI_Init(SPI1);
  //configuración del ETH
  ETH_Reset();
  //CHIP SELECT
  reg_wizchip_cs_cbfunc(ETH_CS_Select, ETH_CS_Unselect);
  //DATOS
  reg_wizchip_spi_cbfunc(ETH_SPI_rb, ETH_SPI_wb);

  wizchip_init(bufSize, bufSize);
  wiz_NetInfo netInfo = { .mac  = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef}, // Mac address
                          .ip   = {192, 168, 16, 130},         // IP address
                          .sn   = {255, 255, 255, 0},         // Subnet mask
                          .gw   = {192, 168, 16, 1}};          // Gateway address
  wizchip_setnetinfo(&netInfo);
  wizchip_getnetinfo(&netInfo);
  //uint8_t mac_address[] = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef};
  //setSHAR(mac_address);

  Network n;
  MQTTClient c;




  NewNetwork(&n, MQTT_SOCKET);
  ConnectNetwork(&n, targetIP, targetPort);
  MQTTClientInit(&c,&n,1000,buf,100,tempBuffer,2048);

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 0;
  data.MQTTVersion = 3;
  data.clientID.cstring = opts.clientid;
  data.username.cstring = "";
  data.password.cstring = "";

  data.keepAliveInterval = 60;
  data.cleansession = 1;

  rc = MQTTConnect(&c, &data);
  printf("Connected %d\r\n", rc);
  opts.showtopics = 1;

  printf("Subscribing to %s\r\n", "hello/wiznet");
  rc = MQTTSubscribe(&c, "hello/wiznet", opts.qos, messageArrived);
  printf("Subscribed %d\r\n", rc);

  MQTTMessage message;
  char payload[50];
  int count =0;
  message.qos = 1;
  message.retained = 0;
  message.payload = payload;
  uint32_t last = millis();
  /* Infinite loop */
  while (1)
  {
	  if((millis()-last)>2000){

//		  if (lwjson_parse(&lwjson, "{\"mykey\":\"myvalue\"}") == lwjsonOK) {
//			  const lwjson_token_t* t;
//			  lwjson_stream_parse(jsp, c)
//			  printf("JSON parsed..\r\n");
//			  printf("%s\r\n",lwjson_get_val_string(tokens, NULL));
////			  lwjson_print_token(tokens);
////			  lwjson_print_json(&lwjson);
////			  /* Find custom key in JSON */
////			  if ((t = lwjson_find(&lwjson, "mykey")) != NULL) {
////				  printf("Key found with data type: %d\r\n", (int)t->type);
////			  }
////
////			  /* Call this when not used anymore */
////			  lwjson_free(&lwjson);
//		  }
		  last = millis();
		  count++;
		  sprintf(payload, "{\"Sensor\":%d}",count);
		  message.payloadlen = strlen(payload);
		  MQTTPublish(&c, "Publish/Topic", &message);
	  }

	  MQTTYield(&c, data.keepAliveInterval);
  }
}


void USART2_GPIOInit(void){
	RCC->AHB1ENR |= GPIOX_CLOCK(USART2_TX);

	GPIOX_MODER(MODE_ALTER, USART2_TX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, USART2_TX);
	GPIOX_AFR(7U, USART2_TX);

	GPIOX_MODER(MODE_ALTER, USART2_RX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, USART2_RX);
	GPIOX_AFR(7U, USART2_RX);
	return;
}


/**
 * @brief configuracion de pines necesarios para el SPI1
 */
void SPI1_GPIOInit(void){
	RCC->AHB1ENR |= GPIOX_CLOCK(SPI1_SCK);
	//CONFIGURAR
	GPIOX_MODER(MODE_ALTER,SPI1_SCK);
	GPIOX_AFR(5,SPI1_SCK);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,SPI1_SCK);

	GPIOX_MODER(MODE_ALTER,SPI1_MISO);
	GPIOX_AFR(5,SPI1_MISO);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,SPI1_MISO);

	GPIOX_MODER(MODE_ALTER,SPI1_MOSI);
	GPIOX_AFR(5,SPI1_MOSI);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,SPI1_MOSI);
	//NSS
	RCC->AHB1ENR |= GPIOX_CLOCK(SPI1_NSS);
	GPIOX_MODER(MODE_OUT, SPI1_NSS);
}


void SPI_Init(SPI_TypeDef *SPIx){
	/*enable clock*/
	if(SPIx == SPI1)
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	else if(SPIx == SPI2)
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	else
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

/******************************************************************************/
	//1. Configurar el baudrate
	SPIx->CR1 &=~ SPI_CR1_BR;			//Fpclk / 2 = 8MHz
	SPIx->CR1 |= SPI_CR1_BR_0;			//Fpclk / 4 = 4MHz
	//2. configurar la poliridad y la fasse del reloj
	//CPOL
	SPIx->CR1 &=~ SPI_CR1_CPOL;			//CPOL = 0
	//CPHA
	SPIx->CR1 &=~ SPI_CR1_CPHA;			//CPHA = 0
	/*3. seleccionar la longitud de la trama*/
	SPIx->CR1 &=~ SPI_CR1_DFF;			//8-bit
	/*4. configurar LSB o MSB first*/
	SPIx->CR1 &=~ SPI_CR1_LSBFIRST;		//MSB FIRST
	/*5. configura el mamejo del pin NSS*/
	SPIx->CR1 |= SPI_CR1_SSM;			//software managment
	SPIx->CR1 |= SPI_CR1_SSI;
	/*6 configurar el modo TI si es que fuera necesario	 */

	/*7. habilita el modo maestro*/
	SPIx->CR1 |= SPI_CR1_MSTR;			//habilita el modo maestro
	/*8. habilitar el spi*/
	SPIx->CR1 |= SPI_CR1_SPE;			//habilita el spi para tx y rx
}

/**
 * @brief SPI send data
 */
void SPI_SendData(SPI_TypeDef *SPIx,uint8_t *pTxBuffer, uint32_t Len){
	while(Len>0){
		//1. esperar hasta que TXE se ponga a 1
		while(!(SPIx->SR & 1U<<1));
		//2. Verificar el bit DFF
		if(SPIx->CR1 & SPI_CR1_DFF){
			//16 bit data
			SPIx->DR = *((uint16_t*)pTxBuffer);
			Len -= 2;
			(uint16_t*)pTxBuffer++;
		}else{
			//8bit
			*((__IO uint8_t*)&SPIx->DR) = *pTxBuffer;
			pTxBuffer++;
			Len--;
		}
	}
	//esperar hasta que el SPI se libere
	while(SPIx->SR & SPI_SR_BSY);
	return;
}

/**
 * @brief transmitirRecibir
 */
void SPI_SendReceiveData(SPI_TypeDef *SPIx,uint8_t *pTxBuffer,uint8_t *pRxBuffer,uint32_t Len){
	while(Len>0){
		//1. esperar hasta que TXE se ponga a 1
		while(!(SPIx->SR & SPI_SR_TXE));
		if(SPIx->CR1 & SPI_CR1_DFF){
			//16 bit data
			SPIx->DR = *((uint16_t*)pTxBuffer);

			//wait for RXNE
			while(!(SPIx->SR & SPI_SR_RXNE));
			*((uint16_t*)pRxBuffer) = SPIx->DR ;

			Len -= 2;
			(uint16_t*)pTxBuffer++;
			(uint16_t*)pRxBuffer++;
		}else{
			*((__IO uint8_t*)&SPIx->DR) = *pTxBuffer;

			//wait for RXNE
			while(!(SPIx->SR & SPI_SR_RXNE));
			*(pRxBuffer) = SPIx->DR;

			pTxBuffer++;
			pRxBuffer++;
			Len--;
		}
	}


	return;
}

/**
 * @brief SPI receive data
 */
void SPI_ReceiveData(SPI_TypeDef *SPIx,uint8_t *pRxBuffer,uint32_t Len){
	while(Len>0){
		//wait until RXNE is set
		while(!(SPIx->SR & SPI_SR_RXNE));

		//2. check the DFF bit in CR1
		if(SPIx->CR1 & SPI_CR1_DFF){
			//16 bit DFF
			//1. load the data from DR to Rxbuffer address
			*((uint16_t*)pRxBuffer) = SPIx->DR;
			Len -= 2;
			(uint16_t*)pRxBuffer++;
		}else{
			//8 bit DFF
			*(pRxBuffer) = SPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
	//esperar hasta que el SPI se libere
	while(SPIx->SR & SPI_SR_BSY);

	return;
}


/**
 * @brief master receive data
 */
void SPI_MasterReceiveData(SPI_TypeDef *SPIx,uint8_t *pRxBuffer,uint32_t Len){

	while(Len>0){
		//1. esperar hasta que TXE se ponga a 1
		while(!(SPIx->SR & SPI_SR_TXE));

		if(SPIx->CR1 & SPI_CR1_DFF){
			//16 bit data
			//the clock is controlled by master
			//thus, the master must send a byte
			SPIx->DR = (uint16_t) 0xFFFF;			//write dummy
			//data to the slave to start the clock
			while(!(SPIx->SR & SPI_SR_RXNE));
			*((uint16_t*)pRxBuffer) = SPIx->DR;

			Len -= 2;
			(uint16_t*)pRxBuffer++;
		}else{
			//8 bit data
			//the clock is controlled by master
			//thus, the master must send a byte
			*((__IO uint8_t*)&SPIx->DR) = 0xFF;		//dummy byte
			//data to the slave to start the clock
			while(!(SPIx->SR & SPI_SR_RXNE));
			*(pRxBuffer) = SPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
	//esperar hasta que el SPI se libere
	while(SPIx->SR & SPI_SR_BSY);
	return;
}
/************************************************************/
/**
 * @brief genera el reset para el ethernet
 */
void ETH_Reset(void){
	GPIOX_ODR(ETH_RST) = 1;
	delay_ms(500);
	GPIOX_ODR(ETH_RST) = 0;
	delay_ms(100);
	GPIOX_ODR(ETH_RST) = 1;
}

void led_blink(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t num_blink, uint32_t delay){
	for(uint8_t i = 0; i<num_blink; i++){
		GPIOx->BSRR |= 1U<<pin;
		delay_ms(delay);
		GPIOx->BSRR |= 1U<<(16 + pin);
		delay_ms(delay);
	}
	return;
}
/************************************************************/
void ETH_CS_Select(void){
	GPIOX_ODR(SPI1_NSS) = 0;
}
void ETH_CS_Unselect(void){
	GPIOX_ODR(SPI1_NSS) = 1;
}
//enviar byte
void ETH_SPI_wb(uint8_t b){
	uint8_t rx;
	SPI_SendReceiveData(SPI1, &b, &rx, 1);
//	SPI_SendData(SPI1, &b, 1);
	return;
}
//recibir byte
uint8_t ETH_SPI_rb(void){
	uint8_t rx;
	SPI_MasterReceiveData(SPI1, &rx, 1);
	//SPI_ReceiveData(SPI1, &rx, 1);
	return rx;
}
/************************************************************/
void messageArrived(MessageData* md)
{
	unsigned char payload[100];

	char *data;
	MQTTMessage* message = md->message;
	printf("-------------------------------------------------\r\n");
	printf("Message arrived [");
	for(uint8_t i = 0; i<md->topicName->lenstring.len; i++){
		printf("%c",md->topicName->lenstring.data[i]);
	}
	printf("]->");
	//data receive
	data =(char*) message->payload;
	for(uint16_t i = 0; i<message->payloadlen; i++){
		payload[i] = data[i];
		printf("%c",payload[i]);
	}
	printf("\r\n");
	printf("QoS------->%d\r\n",message->qos);
	printf("retained-->%d\r\n",message->retained);
	printf("dup------->%d\r\n",message->dup);
	printf("id-------->%d\r\n",message->id);
	printf("-------------------------------------------------\r\n");
}



/************************************************************/
int __io_putchar(int ch){
#if (USE_SWV== 1)
	ITM_SendChar((uint32_t)ch);
#else
	uint8_t c = ch & 0xFF;
	while(!(USART2->SR & USART_SR_TXE));  //espera hasta que usart este listo para transmitir otro byte
	USART2->DR = c;
#endif
	return ch;
}
