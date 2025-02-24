/*
 * servo.c
 *
 *  Created on: Oct 24, 2024
 *      Author: yoyos
 */


#include "servo.h"

ts_servo servo;
uint32_t servo_PCLKxTIM(TIM_HandleTypeDef* ftim);

servo_Result servo_Init(ts_servo* fservoh, TIM_HandleTypeDef* ftim, uint32_t channel, uint32_t u32pwMin,  uint32_t u32pwMax, uint16_t fu16angleMin,  uint16_t fu16angleMax )
{
	servo_Result eResult = SERVO_OK;
	//TODO : verify timer frequency
	if (ftim->State != HAL_TIM_STATE_READY) {
	  // timer is not initialized, or already started
		eResult = SERVO_PARAMETER_ERROR;
	}
	else
	{
		if((ftim->Instance->PSC +1)*1000000 == servo_PCLKxTIM(ftim))//force pwm resolution to 1µs
		{
			if(ftim->Instance->ARR +1 != 20000)//20ms period needed for servo
			{
				eResult = SERVO_PARAMETER_ERROR;
			}
		}
		else
		{
			eResult = SERVO_PARAMETER_ERROR;
		}
	}

	if(eResult == SERVO_OK)
	{
		if((u32pwMin<u32pwMax) && (fu16angleMin<fu16angleMax))
		{
			fservoh->hTimHandle = ftim;
			fservoh->u32Channel = channel;
			fservoh->u32MaxPulseWidth = u32pwMax; //us
			fservoh->u32MinPulseWidth = u32pwMin; //us
			fservoh->u16min_angle = fu16angleMin;
			fservoh->u16max_angle = fu16angleMax;
			HAL_TIM_PWM_Start(fservoh->hTimHandle, fservoh->u32Channel);
			fservoh->status = servoInit;
		}
		else
		{
			eResult = SERVO_PARAMETER_ERROR;
		}
	}
	return eResult;
}

servo_Result servo_Set(ts_servo* fservoh,uint16_t value)
{
	servo_Result eResult = SERVO_OK;
	if(fservoh->status == servoInit)
	{
		if(value<fservoh->u32MinPulseWidth)
		{
			value=fservoh->u32MinPulseWidth;
		}
		if(value>fservoh->u32MaxPulseWidth)
		{
			value=fservoh->u32MaxPulseWidth;
		}
		TIM_OC_InitTypeDef sConfigOC;

		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = value;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		HAL_TIM_PWM_ConfigChannel(fservoh->hTimHandle, &sConfigOC, fservoh->u32Channel);
		HAL_TIM_PWM_Start(fservoh->hTimHandle, fservoh->u32Channel);
		fservoh->u16Value = value;
	}
	else
	{
		eResult = SERVO_INTERNAL_ERROR;
	}
	return eResult;
}

servo_Result servo_TestAmp(ts_servo* fservoh,uint32_t delay)
{
	servo_Result eResult = SERVO_OK;
	if(fservoh->status == servoInit)
	{
		eResult = servo_Set(fservoh,fservoh->u32MinPulseWidth);
		if(eResult == SERVO_OK)
		{
			HAL_Delay(delay);
			eResult = servo_Set(fservoh,fservoh->u32MaxPulseWidth);
		}
	}
	else
	{
		eResult = SERVO_PARAMETER_ERROR;
	}
	return eResult;
}

servo_Result servo_SetAngle(ts_servo* fservoh,float angle)
{
	servo_Result eResult = SERVO_OK;
	uint16_t u16Cmd;

	if(angle>fservoh->u16max_angle)
	{
		angle = (float)fservoh->u16max_angle;
	}
	if(angle<fservoh->u16min_angle)
	{
		angle = (float)fservoh->u16min_angle;
	}
	if(fservoh->status == servoInit)
	{
		u16Cmd = (uint16_t)((float)fservoh->u32MinPulseWidth + (angle-(float)fservoh->u16min_angle)*(fservoh->u32MaxPulseWidth-fservoh->u32MinPulseWidth)/(float)(fservoh->u16max_angle-fservoh->u16min_angle));
		eResult = servo_Set(fservoh,u16Cmd);
	}
	else
	{
		eResult = SERVO_PARAMETER_ERROR;
	}
	return eResult;
}



uint32_t servo_PCLKxTIM(TIM_HandleTypeDef* ftim)
{
  /* Get PCLK1 frequency */
  uint32_t pclkx;
  if(ftim->Instance == TIM3 || ftim->Instance == TIM4 || ftim->Instance == TIM5 || ftim->Instance == TIM6 || ftim->Instance == TIM7)
  {
	  pclkx = HAL_RCC_GetPCLK1Freq();
	  if((RCC->CFGR & RCC_CFGR_PPRE1) != 0)
	  {
		  pclkx *=2;
	  }
  }
  else
  {
	  pclkx = HAL_RCC_GetPCLK2Freq();
	  if((RCC->CFGR & RCC_CFGR_PPRE2) != 0)
	  {
		  pclkx *=2;
	  }
  }
  return pclkx;
}
