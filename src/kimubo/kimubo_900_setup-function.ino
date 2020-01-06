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

	

void setup() {

    // disable ADC and TWI (I2C) and Serial in order to save power
    // power_adc_disable();
    power_twi_disable();
    power_usart0_disable();

    #if defined (debug)
        // initialize serial communication at 9600 bits per second:
        power_usart0_enable();
        Serial.begin(38400);
        Serial.println(F("Hello to the world of KIMUBO!"));
    #endif
    
    setupReadVcc();
    // only continue if voltage is above absolute lower limit
    checkBatteryVoltageAndShutdownIfNeccessary();
    
    
    amp_setup();
    sdc_setup();
    keyb_setup();
    player_setup();
   	


    

    
    #if defined (AUTO_PLAY)
        isFirstLoop = true; // in case of AUTO_PLAY we need to know when we loop for the first time
    #endif

    playerStoppedSince = 0L; // initial value
    



} // setup()
  
// EOF 
