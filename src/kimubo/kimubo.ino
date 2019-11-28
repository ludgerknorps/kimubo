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

#include "local.lib/arduino-fsm/Fsm.h"
#include "local.lib/arduino-fsm/Fsm.cpp"

#include "local.lib/PetitFS/src/PetitFS.h"
#include "local.lib/PetitFS/src/avr_mmcp.cpp"
#include "local.lib/PetitFS/src/diskio.h"
#include "local.lib/PetitFS/src/integer.h"
#include "local.lib/PetitFS/src/pff.cpp"
#include "local.lib/PetitFS/src/pff.h"
#include "local.lib/PetitFS/src/pffArduino.h"
#include "local.lib/PetitFS/src/pffconf.h"

// wir verwenden das Arduino Pro Mini interne EEPROM zum speichern von Einstellungen
#include <EEPROM.h>



// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt finite state machine smMain
 */
 
	// see kimubo_100_fsmMain.ino
		
		
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt Audio / PCM Player
 */
		
	// see kimubo_020_pcmplayer.ino


	
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt MatrixKeyboard
 */
 
	// see kimubo_070_keypad.ino
		


// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt UBat measurement
 */
 
	
	
		/* some globals */
		
		// TBD. put into globals
		
		
		
		
		
	// see kimubo_060_ubat.ino
		
		
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt Messages
 */

	// see kimubo_090_messages.ino


// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt SETUP
 */

	// see kimubo_900_setup-function.ino


 

	
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################
/* 
 * Abschnitt LOOP
 */
	
	// see kimubo_950_loop-function.ino
