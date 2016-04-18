/*
*********************************************************************************************************
*	                                  
*	模块名称 : 按键驱动模块
*	文件名称 : bsp_key.c
*	版    本 : V1.0
*	说    明 : 实现按键的检测，具有软件滤波机制，可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*				(5) 组合键
*
*
*********************************************************************************************************
*/

#include "LPC17xx.h" 
#include <stdio.h>

#include "bsp_key.h"

/**********************************************************************
*	                                  
	按键口线分配：
	KEY1键	: P1.18	(低电平表示按下)
*
*********************************************************************/
/*接口定义*/

/*PIN*/
#define PIN_KEY		1 << 18

/*局部变量*/
static BUTTON_T s_BtnSet;		/* SET键 */
static KEY_FIFO_T s_Key;		/* 按键FIFO变量,结构体 */

/*供内部调用*/
static void bsp_InitButtonVar(void);
static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

/*	定义函数判断按键是否按下，返回值1 表示按下，0表示未按下*/
uint8_t IsKeyDownSet(void)	{if ((LPC_GPIO1->FIOPIN)&PIN_KEY) return 0; return 1;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitButton
*	功能说明: 初始化按键
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitButton(void)
{
	bsp_InitButtonVar();		/* 初始化按键变量 */
	bsp_InitButtonHard();		/* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参：_KeyCode : 按键代码
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];

		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyState
*	功能说明: 读取按键的状态
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_KeyState(uint8_t _ucKeyID)
{
	uint8_t ucState = 0;

	switch (_ucKeyID)
	{
		case KID_SET:
			ucState = s_BtnSet.State;
			break;
	}

	return ucState;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitButtonHard
*	功能说明: 初始化按键硬件
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitButtonHard(void)
{
	/*
	按键口线分配：
	SET键	: P1.18	(低电平表示按下)
	*/

    LPC_PINCON->PINSEL3 &= ~(0x03<<4);//bit4,5 
    LPC_GPIO1->FIODIR   &= ~(PIN_KEY); 
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitButtonVar
*	功能说明: 初始化按键变量
*	形    参：strName : 例程名称字符串
*			  strDate : 例程发布日期
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitButtonVar(void)
{
	/* 对按键FIFO读写指针清零 */
	s_Key.Read = 0;
	s_Key.Write = 0;	

	/* 初始化SET按键变量，支持按下、连发（周期10ms） */
	s_BtnSet.IsKeyDownFunc = IsKeyDownSet;			/* 判断按键按下的函数 */
	s_BtnSet.FilterTime = BUTTON_FILTER_TIME;		/* 按键滤波时间 */
	s_BtnSet.LongTime = BUTTON_LONG_TIME;			/* 长按时间 */
	s_BtnSet.Count = s_BtnSet.FilterTime / 2;		/* 计数器设置为滤波时间的一半 */
	s_BtnSet.State = 0;								/* 按键缺省状态，0为未按下 */
	s_BtnSet.KeyCodeDown = KEY_DOWN_SET;			/* 按键按下的键值代码 */
	s_BtnSet.KeyCodeUp = KEY_UP_SET;				/* 按键弹起的键值代码，0表示不检测 */
	s_BtnSet.KeyCodeLong = KEY_LONG_SET;			/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnSet.RepeatSpeed = 0;						/* 按键连发的速度，0表示不支持连发 */
	s_BtnSet.RepeatCount = 0;						/* 连发计数器 */		
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectButton
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参：按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectButton(BUTTON_T *_pBtn)
{
	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;

				/* 发送按钮按下的消息 */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* 键值放入按键FIFO */
					bsp_PutKey(_pBtn->KeyCodeDown);
				}
			}

			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						bsp_PutKey(_pBtn->KeyCodeLong);						
					}
				}
				else
				{
					if (_pBtn->RepeatSpeed > 0)
					{
						if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
						{
							_pBtn->RepeatCount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_PutKey(_pBtn->KeyCodeDown);														
						}
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* 键值放入按键FIFO */
					bsp_PutKey(_pBtn->KeyCodeUp);			
				}
			}
		}

		_pBtn->LongCount = 0;
		_pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyPro
*	功能说明: 检测所有按键。非阻塞状态，必须被周期性的调用。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyPro(void)
{
	bsp_DetectButton(&s_BtnSet);			/* SET键 */
}
