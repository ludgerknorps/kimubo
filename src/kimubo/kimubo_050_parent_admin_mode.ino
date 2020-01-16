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

	// global variable to hold the parent settings from/to EEPROM
	// bit 0 == 	loudness == 		( gv_parentAdminModeSettings >> 0) & 1 )
	// bit 1 == 	greeting == 		( gv_parentAdminModeSettings >> 1) & 1 )
	// bit 2 == 	autoplay == 		( gv_parentAdminModeSettings >> 2) & 1 )
	byte gv_parentAdminModeSettings;


	/* =========================================================== */
	bool setupParentAdminSettingsFromEEPROM(){
		
		// STEP 0: Read AdminSettings from EEPROM 
		EEPROM.get(EEPROM_PARENTADMINMODE_SETTINGS_ADDR, gv_parentAdminModeSettings); // this is one byte
	}
	
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

		/*
		 * ok, what can the (parent) user do in this parent admin mode?
		 * 
		 * a) press key1 (upper left) --> device tells the current battery voltage to user
		 * 
		 * b)
		 *    press key 2 (upper middle) repeatedly --> toggle loudness:
		 * 		first press: device tells the current loudness setting to user
		 * 		each following press: loudness toggles and device tells new setting
		 * 		
		 * 		loudness has the following states: OFF, ON
		 * 		
		 * c) TBD LATER! 
		 *    press key 3 (upper right) repeatedly --> toggle sleepTimer:
		 * 		first press: device tells the current sleepTimer setting to user
		 * 		each following press: sleepTimer toggles and device tells new setting
		 * 		
		 * 		sleepTimer has the following states: OFF, 15min, 30min
		 * 		
		 * d)
		 *    press key 4 (middle left) repeatedly --> toggle Greeting:
		 * 		first press: device tells the current Greeting setting to user
		 * 		each following press: Greeting toggles and device tells new setting
		 * 		
		 * 		Greeting has the following states: OFF, ON-FIXED, ON-RANDOM
		 * 		where 	ON-FIXED will always play file 254.WAV and 
		 * 				ON-RANDOM plays (randomly) one of 245.WAV .. 254.WAV
		 * 		ON-RANDOM will be implemented later!
		 * 		
		 * e)
		 *    press key 5 (middle middle) repeatedly --> toggle Autoplay:
		 * 		first press: device tells the current Autoplay setting to user
		 * 		each following press: Autoplay toggles and device tells new setting
		 * 		
		 * 		Autoplay has the following states: OFF, ON
		 * 		
		 * 	
		 * 		
		 * other than in normal playback mode the actions are carried out at pressing the key (compared to releasing a key)
		 */

		char c;

		keyb_waitForAllKeysToBeReleased();

		// we only leave parent admin mode by power-off-on-reset
		while (1){

			c = keypad.waitForKey();
		
			if ( c == KEYSCAN_1 ) {
				parentAdminModeReadVcc();
				
			} else if ( c == KEYSCAN_2 ) {
				
				// toggle loudness
				if ( readParentAdminSettingLoudness() ) {
					clrParentAdminSettingLoudness();
					playMessage(message_loudness_off, sizeof(message_loudness_off));
				} else {
					setParentAdminSettingLoudness();
					playMessage(message_loudness_on, sizeof(message_loudness_on));
				}
			} else if ( c == KEYSCAN_4 ) {
				
				// toggle greeting
				if ( readParentAdminSettingGreeting() ) {
					clrParentAdminSettingGreeting();
					playMessage(message_greeting_off, sizeof(message_greeting_off));
				} else {
					setParentAdminSettingGreeting();
					playMessage(message_greeting_on, sizeof(message_greeting_on));
				}
			} else if ( c == KEYSCAN_5 ) {
				
				// toggle greeting
				if ( readParentAdminSettingAutoplay() ) {
					clrParentAdminSettingAutoplay();
					playMessage(message_autoplay_off, sizeof(message_autoplay_off));
				} else {
					setParentAdminSettingAutoplay();
					playMessage(message_autoplay_on, sizeof(message_autoplay_on));
				}
			}

			keyb_waitForAllKeysToBeReleased();
			
		}
		
	} // parentAdminModeLoop()
