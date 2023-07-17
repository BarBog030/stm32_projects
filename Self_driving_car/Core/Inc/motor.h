/*
 * l298n.h
 *
 *  Created on: Jul 15, 2023
 *      Author: barbog
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "pinmap.h"

enum
{
	MOTOR_ONEEIGHTSPEED = 4000,
	MOTOR_QUARTERSPEED = 8000,
	MOTOR_HALFSPEED = 16000,
	MOTOR_FULLSPEED = 32000
};

class Motor
{
	private:
		pinStruct_t pin1;
		pinStruct_t pin2;
		TIM_HandleTypeDef htim;
		uint8_t pwmCh;

	public:
		Motor(pinStruct_t* motorPin1,
			  pinStruct_t* motorPin2,
			  pinStruct_t* pwmPin,
			  TIM_TypeDef* TIMx,
			  uint8_t gpioAFSelTIMx,
			  uint8_t pwmChannel);

		void MoveForward(uint32_t speed);
		void MoveBackward(uint32_t speed);
		void Stop(void);
};


#endif /* INC_MOTOR_H_ */
