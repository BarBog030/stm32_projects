/*
 * game.h
 *
 *  Created on: Jul 4, 2023
 *      Author: barbog
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "lcd.h"
#include "keypad.h"

extern void Game_DisplayIntro(LCD* lcd);
extern void Game_DisplayMenu(LCD* lcd);
extern void Game_DisplayRules(LCD* lcd);
extern void Game_Start(LCD* lcd, Keypad* keypad);


#endif /* INC_GAME_H_ */
