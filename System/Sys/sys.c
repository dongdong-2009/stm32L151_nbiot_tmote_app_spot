/**
  *********************************************************************************************************
  * @file    sys.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "sys.h"
#include "platform_config.h"
#include "platform_map.h"
#include "tmesh_algorithm.h"

__IO u32 SystemRunningTimesSecond = 0;										//系统运行时间(S)
__IO u32 SystemSoftResetTime = 0;											//系统运行超时软件复位时间(MS)
Stm32_SystemRunningTime		SystemRunningTime;								//系统运行时间(结构体)

/**********************************************************************************************************
 @Function			void Stm32_Clock_Init(u32 pllmul, u32 plldiv)
 @Description			时钟设置函数(MAX32MHz)
 @Input				pllmul	: 主PLL倍频系数(PLL倍频),取值范围:0 ~ 48
					plldiv	: 系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,3,4.(仅限这3个值!)
 @Return				void
 @attention			Fsys = Fs*(pllmul/plldiv);

					Fsys : 系统时钟频率
					Fs	: PLL输入时钟频率,可以是HSI,HSE等
**********************************************************************************************************/
void Stm32_Clock_Init(u32 pllmul, u32 plldiv)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_ClkInitTypeDef RCC_ClkInitStructure;
	
	HAL_RCC_DeInit();

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;				//时钟源为HSI
	RCC_OscInitStructure.HSIState = RCC_HSI_ON;								//打开HSI
	RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;		//HSI默认校准
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;							//打开PLL
	RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSI;					//PLL时钟源选择HSI
	RCC_OscInitStructure.PLL.PLLMUL = pllmul;								//主PLL倍频系数(PLL倍频)
	RCC_OscInitStructure.PLL.PLLDIV = plldiv;								//系统时钟的主PLL分频系数(PLL之后的分频)
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//初始化
	if (ret != HAL_OK) while(1);
	
	__HAL_RCC_PWR_CLK_ENABLE();											//使能PWR时钟
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);				//设置调压器输出电压级别, 以便在器件未以最大频率工作
	while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};

	/* 选中PLL作为系统时钟源并且配置HCLK, PCLK1 和 PCLK2 */
	RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;				//设置系统时钟时钟源为PLL
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;						//AHB分频系数为1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV1;						//APB1分频系数为1
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;						//APB2分频系数为1

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_1);			//同时设置FLASH延时周期为1WS, 也就是2个CPU周期
	if (ret != HAL_OK) while(1);
}

/**********************************************************************************************************
 @Function			void Stm32_MSIClock_Init(u32 msiClockRange)
 @Description			时钟设置函数(MAX32MHz)
 @Input				msiClockRange : 
 @attention			RCC_MSIRANGE_0				!< MSI = 65.536  KHz
					RCC_MSIRANGE_1				!< MSI = 131.072 KHz
					RCC_MSIRANGE_2				!< MSI = 262.144 KHz
					RCC_MSIRANGE_3				!< MSI = 524.288 KHz
					RCC_MSIRANGE_4				!< MSI = 1.048   MHz
					RCC_MSIRANGE_5				!< MSI = 2.097   MHz
					RCC_MSIRANGE_6				!< MSI = 4.194   MHz
  @Return				void
**********************************************************************************************************/
void Stm32_MSIClock_Init(u32 MsiClockRange)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_ClkInitTypeDef RCC_ClkInitStructure;
	
	HAL_RCC_DeInit();
	
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_MSI;				//时钟源为MSI
	RCC_OscInitStructure.MSIState = RCC_MSI_ON;								//打开MSI
	RCC_OscInitStructure.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;		//MSI默认校准
	RCC_OscInitStructure.MSIClockRange = MsiClockRange;						//MSIValue
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_OFF;							//关闭PLL
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//初始化
	if (ret != HAL_OK) while(1);
	
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;				//时钟源为HSI --> ADC
	RCC_OscInitStructure.HSIState = RCC_HSI_ON;								//打开HSI
	RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;		//HSI默认校准
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_OFF;							//关闭PLL
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//初始化
	if (ret != HAL_OK) while(1);
	
	__HAL_RCC_PWR_CLK_ENABLE();											//使能PWR时钟
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);				//设置调压器输出电压级别, 以便在器件未以最大频率工作
	while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
	
	/* 选中MSI作为系统时钟源并且配置HCLK, PCLK1 和 PCLK2 */
	RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;					//设置系统时钟时钟源为MSI
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;						//AHB分频系数为1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV1;						//APB1分频系数为1
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;						//APB2分频系数为1
	
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_1);			//同时设置FLASH延时周期为1WS, 也就是2个CPU周期
	if (ret != HAL_OK) while(1);
}

/**********************************************************************************************************
 @Function			void Stm32_System_Software_Reboot(void)
 @Description			Stm32_System_Software_Reboot : 软重启
 @Input				void
 @Return				void
**********************************************************************************************************/
void Stm32_System_Software_Reboot(void)
{
	TCFG_SystemData.MagBackgroundX = Qmc5883lData.X_Back;
	TCFG_SystemData.MagBackgroundY = Qmc5883lData.Y_Back;
	TCFG_SystemData.MagBackgroundZ = Qmc5883lData.Z_Back;
	TCFG_EEPROM_SetMagBackgroud(TCFG_SystemData.MagBackgroundX, TCFG_SystemData.MagBackgroundY, TCFG_SystemData.MagBackgroundZ);
	
	__set_FAULTMASK(1);													//关闭所有中断
	HAL_NVIC_SystemReset();												//软件复位
	__NOP();
}

/**********************************************************************************************************
 @Function			void Stm32_IncSecondTick(void)
 @Description			Stm32_IncSecondTick : 运行时间加1秒
 @Input				void
 @Return				void
**********************************************************************************************************/
void Stm32_IncSecondTick(void)
{
	static u32 SecondTickPre;
	u32 SecondTickNow;
	
	SecondTickNow = HAL_GetTick();
	if ((SecondTickNow > SecondTickPre) && ((SecondTickNow - SecondTickPre) >= 1000)) {
		SystemRunningTimesSecond++;
		SecondTickPre += 1000;
	}
	else if ((0xFFFFFFFF - SecondTickPre + SecondTickNow + 1) >= 1000) {
		SystemRunningTimesSecond++;
		SecondTickPre += 1000;
	}
}

/**********************************************************************************************************
 @Function			u32 Stm32_GetSecondTick(void)
 @Description			Stm32_GetSecondTick 	: 获取运行秒数
 @Input				void
 @Return				SystemRunningTimesSecond : 运行秒数
**********************************************************************************************************/
u32 Stm32_GetSecondTick(void)
{
	return SystemRunningTimesSecond;
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_Init(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_Init 			: 初始化计时器
 @Input				Stm32_CalculagraphTypeDef*		: 计时器结构体指针
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_Init(Stm32_CalculagraphTypeDef* timer)
{
	timer->xTicksToWait = 0;
	timer->xTimeOut = 0;
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_CountdownMS(Stm32_CalculagraphTypeDef* timer, u32 timeout_ms)
 @Description			Stm32_Calculagraph_CountdownMS	: 配置计时器倒数时间(MS)
 @Input				Stm32_CalculagraphTypeDef*		: 计时器结构体指针
					timeout_ms					: 倒计时时间(MS)
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_CountdownMS(Stm32_CalculagraphTypeDef* timer, u32 timeout_ms)
{
	timer->xTicksToWait = timeout_ms;
	timer->xTimeOut = HAL_GetTick() + timeout_ms;
}

/**********************************************************************************************************
 @Function			bool Stm32_Calculagraph_IsExpiredMS(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_IsExpiredMS	: 查询是否到达计时器计时时间(MS)
 @Input				Stm32_CalculagraphTypeDef*		: 计时器结构体指针
 @Return				true							: 到达
					false						: 未到达
**********************************************************************************************************/
bool Stm32_Calculagraph_IsExpiredMS(Stm32_CalculagraphTypeDef* timer)
{
	u32 MilliSecondTickNow;
	
	MilliSecondTickNow = HAL_GetTick();
	
	if ((MilliSecondTickNow > (timer->xTimeOut - timer->xTicksToWait)) && ((MilliSecondTickNow - (timer->xTimeOut - timer->xTicksToWait)) >= timer->xTicksToWait)) {
		return true;
	}
	else if ((0xFFFFFFFF - (timer->xTimeOut - timer->xTicksToWait) + MilliSecondTickNow + 1) >= timer->xTicksToWait) {
		return true;
	}
	else {
		return false;
	}
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_CountdownSec(Stm32_CalculagraphTypeDef* timer, u32 timeout_sec)
 @Description			Stm32_Calculagraph_CountdownSec	: 配置计时器倒数时间(S)
 @Input				Stm32_CalculagraphTypeDef*		: 计时器结构体指针
					timeout_sec					: 倒计时时间(S)
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_CountdownSec(Stm32_CalculagraphTypeDef* timer, u32 timeout_sec)
{
	timer->xTicksToWait = timeout_sec;
	timer->xTimeOut = Stm32_GetSecondTick() + timeout_sec;
}

/**********************************************************************************************************
 @Function			bool Stm32_Calculagraph_IsExpiredSec(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_IsExpiredSec	: 查询是否到达计时器计时时间(S)
 @Input				Stm32_CalculagraphTypeDef*		: 计时器结构体指针
 @Return				true							: 到达
					false						: 未到达
**********************************************************************************************************/
bool Stm32_Calculagraph_IsExpiredSec(Stm32_CalculagraphTypeDef* timer)
{
	u32 SecondTickNow;
	
	SecondTickNow = Stm32_GetSecondTick();
	
	if ((SecondTickNow > (timer->xTimeOut - timer->xTicksToWait)) && ((SecondTickNow - (timer->xTimeOut - timer->xTicksToWait)) >= timer->xTicksToWait)) {
		return true;
	}
	else if ((0xFFFFFFFF - (timer->xTimeOut - timer->xTicksToWait) + SecondTickNow + 1) >= timer->xTicksToWait) {
		return true;
	}
	else {
		return false;
	}
}


#ifdef  USE_FULL_ASSERT
/**********************************************************************************************************
 @Function			void assert_failed(uint8_t* file, uint32_t line)
 @Description			当编译提示出错的时候此函数用来报告错误的文件和所在行
 @Input				file		: 指向源文件
					line		: 指向在文件中的行数
 @Return				void
**********************************************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1)
	{
	}
}
#endif

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

/********************************************** END OF FLEE **********************************************/
