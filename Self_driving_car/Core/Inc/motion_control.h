/*
 * motion_control.h
 *
 *  Created on: Jul 15, 2023
 *      Author: barbog
 */

#ifndef INC_MOTION_CONTROL_H_
#define INC_MOTION_CONTROL_H_

#include "motor.h"
#include "hcsr04.h"

extern void MoveToFreePath(HCSR04* left,
						   HCSR04* mid,
						   HCSR04* right,
						   const uint32_t* minObstacleDistance,
						   Motor* leftMotor,
						   Motor* rightMotor);


#endif /* INC_MOTION_CONTROL_H_ */
