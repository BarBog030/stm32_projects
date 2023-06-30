/*
 * servo.h
 *
 *  Created on: Jun 23, 2023
 *      Author: barbog
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "pinmap.h"

enum
{
	SERVO_RIGHTPOSITION = 4000,
	SERVO_MIDPOSITION = 2400,
	SERVO_LEFTPOSITION = 800
};

class Servo
{
	private:
		TIM_HandleTypeDef htim;
		uint8_t pwmCh;
	public:
		Servo(pinStruct_t* servoPin,
			  TIM_TypeDef* TIMx,
			  uint8_t gpioAFSelTIMx,
			  uint8_t pwmChannel);

		void TurnShafts(uint32_t position);
};



#endif /* INC_SERVO_H_ */
