/*
 * servo.h
 *
 *  Created on: Oct 24, 2024
 *      Author: yoyos
 */

#ifndef YHLIB_SERVO_SERVO_H_
#define YHLIB_SERVO_SERVO_H_

#include "servo_cfg.h"

typedef enum {
	SERVO_OK, SERVO_PARAMETER_ERROR, SERVO_INTERNAL_ERROR, SERVO_RESULTNB
} servo_Result;

typedef enum{
	servoIdle = 0,
	servoInit
}eServo_Status;

typedef struct {
	TIM_HandleTypeDef   *hTimHandle;
	uint32_t			u32Channel;
	uint32_t			u32MaxPulseWidth; //us
	uint32_t			u32MinPulseWidth; //us
	uint16_t			u16min_angle;
	uint16_t			u16max_angle;
	uint16_t 			u16Value;
	eServo_Status		status;
}ts_servo;

servo_Result servo_Init(ts_servo* fservoh, TIM_HandleTypeDef* ftim, uint32_t channel, uint32_t u32pwMin,  uint32_t u32pwMax, uint16_t fu16angleMin,  uint16_t fu16angleMax );
servo_Result servo_Set(ts_servo* fservoh,uint16_t value);
servo_Result servo_TestAmp(ts_servo* fservoh,uint32_t delay);
servo_Result servo_SetAngle(ts_servo* fservoh,float angle);



#endif /* YHLIB_SERVO_SERVO_H_ */
