/*
 * lcd.cpp
 *
 *  Created on: Jul 1, 2023
 *      Author: barbog
 */

#include "lcd.h"

enum LCDCommands
{
	FUNCTION_SET_8BIT = 0x03,
	FUNCTION_SET_4BIT = 0x02,
	FUNCTION_SET_2LINE_5x8DOT = 0x28,
	CLEAR_DISPLAY = 0x01,
	DISPLAY_ON_CURSOR_ON = 0x0E,
	DISPLAY_ON_CURSOR_OFF = 0x0C,
	ENTRY_MODE_INCREMENT_CURSOR = 0x06
};

enum NibbleBitPosition
{
	LOW_NIBBLE = 0,
	HIGH_NIBBLE = 4
};

const uint8_t ddramAddr[4][20] =
{
	{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13},
	{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53},
	{0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27},
	{0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67}
};

static void IntegerToString(uint32_t integer, char* pBuffer)
{
	if(integer == 0)
	{
		// Edge case
		pBuffer[0] = '0';
		return;
	}

	uint32_t copyOfInt = integer;
	uint8_t numberOfDigits = 0;

	// get number of digits
	while(copyOfInt > 0)
	{
		copyOfInt /= 10;
		numberOfDigits++;
	}

	// get each digit, convert to character and store in buffer
	while(integer > 0)
	{
		pBuffer[numberOfDigits - 1] = '0' + (integer % 10);
		integer /= 10;
		numberOfDigits--;
	}
}

/*
 * Example of Write high nibble
 * byte: D7 D6 D5 D4
 * byte: 1  0  1  1  0101, nibbleBitPos = 4
 *
 * nibbleArray = {D4, D5, D6, D7}
 * nibbleArray = {1,  1,  0,  1}
 *
 * Send nibble to lcd:
 * Write nibbleArray[0] to D4
 * Write nibbleArray[1] to D5
 * Write nibbleArray[2] to D6
 * Write nibbleArray[3] to D7
*/

void LCD::WriteNibble(char byte, uint8_t nibbleBitPos)
{
	uint8_t nibbleArray[4] = {0};
	uint8_t j = 0;

	for(uint8_t i = nibbleBitPos; i < nibbleBitPos + 4; i++)
	{
		nibbleArray[j] = (byte & (1<<i)) >> i;
		j++;
	}

	// Send nibble to lcd pins
	for(uint8_t i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(dataPins[i].port, dataPins[i].selectedPin, (GPIO_PinState)nibbleArray[i]);
	}

	// High to low pulse on EN pin (to transfer nibble)
	HAL_GPIO_WritePin(en.port, en.selectedPin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(en.port, en.selectedPin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void LCD::WriteByte(GPIO_PinState lcdMode, char byte)
{
	HAL_GPIO_WritePin(rs.port, rs.selectedPin, lcdMode); // Register select
	LCD::WriteNibble(byte, HIGH_NIBBLE);
	LCD::WriteNibble(byte, LOW_NIBBLE);
}

void LCD::WriteBytes(const char* pData)
{
	while(*pData != '\0')
	{
		LCD::WriteByte(GPIO_PIN_SET, *pData);
		pData++;
	}
}

void LCD::WriteInteger(uint32_t data)
{
	const uint8_t maxNumberOfDigits = 10;
	char IntegerToStringBuffer[maxNumberOfDigits] = {0};

	if(data < 10)
	{
		LCD::WriteByte(GPIO_PIN_SET, '0'); // Optional alignment of digits 0 - 9
	}

	IntegerToString(data, IntegerToStringBuffer);
	LCD::WriteBytes(IntegerToStringBuffer);
}

LCD::LCD(pinStruct_t* RS,
		 pinStruct_t* EN,
		 pinStruct_t* D4,
		 pinStruct_t* D5,
		 pinStruct_t* D6,
		 pinStruct_t* D7)
{
	// LCD pin initialization
	GPIO_InitTypeDef lcdPinInitStruct[6] = {0};
	pinStruct_t* lcdPinArray[6] = {RS, EN, D4, D5, D6, D7};
	for(uint8_t i = 0; i < 6; i++)
	{
		lcdPinInitStruct[i].Pin = lcdPinArray[i]->selectedPin;
		lcdPinInitStruct[i].Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(lcdPinArray[i]->port, &lcdPinInitStruct[i]);
	}

	// Private variables initialization
	rs = *RS;
	en = *EN;
	dataPins[0] = *D4;
	dataPins[1] = *D5;
	dataPins[2] = *D6;
	dataPins[3] = *D7;

	// LCD initialization sequence
	HAL_Delay(16); // Power-on delay
	LCD::WriteByte(GPIO_PIN_RESET, FUNCTION_SET_8BIT); // Function Set mode (8-bit mode)
	HAL_Delay(5); // wait for more than 4.1 ms
	LCD::WriteByte(GPIO_PIN_RESET, FUNCTION_SET_8BIT); // Function Set mode (8-bit mode)
	HAL_Delay(1); // wait form more than 100 us

	// 4-bit operation commands
	uint8_t fourBitCommandSetting[5] =
	{
		FUNCTION_SET_4BIT,
		FUNCTION_SET_2LINE_5x8DOT,
		CLEAR_DISPLAY,
		DISPLAY_ON_CURSOR_OFF,
		ENTRY_MODE_INCREMENT_CURSOR
	};
	for(uint8_t i = 0; i < 5; i++)
	{
		LCD::WriteByte(GPIO_PIN_RESET, fourBitCommandSetting[i]);
	}
}

void LCD::SetCursor(uint8_t row, uint8_t column)
{
	if ((row > 3) || (column > 19))
	{
		// Out of range
		return;
	}

	LCD::WriteByte(GPIO_PIN_RESET, ((1<<7) | ddramAddr[row][column]));
}

void LCD::Print(char data)
{
	LCD::WriteByte(GPIO_PIN_SET, data);
}

void LCD::Print(const char* pData)
{
	LCD::WriteBytes(pData);
}

void LCD::Print(uint8_t* data)
{
	LCD::WriteInteger(*data);
}

void LCD::Print(uint16_t* data)
{
	LCD::WriteInteger(*data);
}

void LCD::Print(uint32_t* data)
{
	LCD::WriteInteger(*data);
}

void LCD::Clear(void)
{
	LCD::WriteByte(GPIO_PIN_RESET, CLEAR_DISPLAY);
}



