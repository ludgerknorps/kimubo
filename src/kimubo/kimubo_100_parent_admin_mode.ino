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

  	/* =========================================================== */
	bool checkForParentAdminModeAtDeviceStartup(){
		// we enter parent admin mode by having two keys pressed at startup --> getkey*S*()
		delay(50); // possibly not necessary but to be safe for debouncing...
		keypad.getKeys();
		
		return ( 	(keypad.countPressed() == 2) && 
					(keypad.findInList((char)KEYSCAN_1) != -1) && 
					(keypad.findInList((char)KEYSCAN_4) != -1) 
				);
	}

		/* =========================================================== */
	void parentAdminModeReadVcc(){
		batteryVoltageAsMessageToParents();
	}

	/* =========================================================== */
	// this function blocks until PowerOff-PowerOn Reset!!
	void parentAdminModeLoop(){

		char c;

		// wait for all keys to be released;
		while (keypad.countPressed() > 0) {
			keypad.getKeys();
		}
	
		while (1){

			c = keypad.waitForKey();
		
			if ( c == KEYSCAN_1 ) {
				parentAdminModeReadVcc();
			}

			// wait for all keys to be released;
			while (keypad.countPressed() > 0) {
				keypad.getKeys(); // do nothing but waiting...
			}
			
		}
		
	} // parentAdminModeLoop()
