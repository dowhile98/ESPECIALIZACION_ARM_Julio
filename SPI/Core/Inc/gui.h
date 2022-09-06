//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F407ZGT6,����ԭ��Explorer STM32F4������,��Ƶ168MHZ������12MHZ
//QDtech-OLEDҺ������ for STM32
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/8/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
// OLEDģ��               STM32��Ƭ��
//   VCC         ��       DC 5V/3.3V      //OLED����Դ��
//   GND         ��          GND          //OLED����Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������Ϊ4����SPI
// OLEDģ��               STM32��Ƭ��
//   D1          ��          PB5         //OLED��SPIд�ź�
//=======================================Һ���������߽���==========================================//
// OLEDģ��               STM32��Ƭ��
//   CS          ��          PB15        //OLED��Ƭѡ�����ź�
//   RES         ��          PB12        //OLED����λ�����ź�
//   DC          ��          PB14        //OLED������/����ѡ������ź�
//   D0          ��          PB3         //OLED��SPIʱ���ź�
//=========================================����������=========================================//
//��ģ�鲻���������ܣ����Բ���Ҫ����������
****************************************************************************************************/	
/***************************************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
****************************************************************************************************/			
#ifndef __GUI_H__
#define __GUI_H__

#include "oled.h"

void GUI_DrawPoint(u8 x, u8 y, u8 color);
void GUI_Fill(u8 sx,u8 sy,u8 ex,u8 ey,u8 color);
void GUI_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2,u8 color);
void GUI_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2,u8 color);
void GUI_FillRectangle(u8 x1, u8 y1, u8 x2, u8 y2,u8 color);
void GUI_DrawCircle(u8 xc, u8 yc, u8 color, u8 r);
void GUI_FillCircle(u8 xc, u8 yc, u8 color, u8 r);
void GUI_DrawTriangel(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2,u8 color);
void GUI_FillTriangel(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2,u8 color);
void GUI_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 mode);
void GUI_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Size,u8 mode);
void GUI_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size,u8 mode);
void GUI_ShowFont16(u8 x,u8 y,u8 *s,u8 mode);
void GUI_ShowFont24(u8 x,u8 y,u8 *s,u8 mode);
void GUI_ShowFont32(u8 x,u8 y,u8 *s,u8 mode);
void GUI_ShowCHinese(u8 x,u8 y,u8 hsize,u8 *str,u8 mode);
void GUI_DrawBMP(u8 x,u8 y,u8 width, u8 height, u8 BMP[], u8 mode); 
#endif

