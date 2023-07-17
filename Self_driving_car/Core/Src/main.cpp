#include "motion_control.h"

int main(void)
{
	HAL_Init();

	return 1;
}

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
}
