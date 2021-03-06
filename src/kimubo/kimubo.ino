/* 
 * KIMUBO
 * KInderMUsikBOx
 * 
 * Ein minimalistischer WAV/PCM Spieler für Kinder - und mehr.
 * 
 * (c) 2019 ludgerknorps <56650955+ludgerknorps@users.noreply.github.com>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. See file "LICENSE". 
 *   If not, see <https://www.gnu.org/licenses/>.
 * 
 */
 



#include "kimubo.h"

//adapted version of libraries!
#include "local.lib/Keypad/src/Keypad.h"
#include "local.lib/Keypad/src/Keypad.cpp"
#include "local.lib/Keypad/src/Key.h"
#include "local.lib/Keypad/src/Key.cpp"

// wir verwenden das Arduino Pro Mini interne EEPROM zum speichern von Einstellungen
#include <EEPROM.h>

#include <SdFat.h>              // need to include the SD library
#include <LKpcm.h>    

// Power saving techniques
#include <avr/power.h>
#include <avr/sleep.h>
