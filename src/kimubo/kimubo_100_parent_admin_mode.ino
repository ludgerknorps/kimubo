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
		return keypad.isPressed(KEYSCAN_1);
		// add further keys here later if necessary like in 
		// 		return keypad.isPressed(KEYSCAN_1) || keypad.isPressed(KEYSCAN_2)
	}

	/* =========================================================== */
	// this function blocks loop() until PowerOff-PowerOn Reset!!
	void runParentAdminMode(){

		delay(500); // wait a short time for AMP and so on to settle if messages is all we play...
		
		if ( keypad.isPressed(KEYSCAN_1) ) {
			parentAdminModeReadVcc();
		}


		while (1); // block device until PowerOff-PowerOn Reset!!
		
	} // runParentAdminMode()

	

	/* =========================================================== */
	void parentAdminModeReadVcc(){
		batteryVoltageAsMessageToParents();
	}
