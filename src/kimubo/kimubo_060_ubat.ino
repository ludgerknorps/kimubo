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
 
// tbd. later
 

//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ // ####################################################################################
//~ /* 
 //~ * Abschnitt UBat measurement
 //~ */
 
	//~ // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//~ // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//~ // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//~ // Read 1.1V reference against AVcc
	//~ // taken from http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
	//~ // 
	//~ // blocking does not matter to us because we measure between tracks -->  some ms don't matter at all :-)
	
		
		
		//~ /* =========================================================== */
		//~ // called once at device startup
		//~ void setupReadVcc(){
			//~ // STEP 0: Read gv_smUBat_ChipsVccCompensationValue from EEPROM (see below for explanation on that value!)
			//~ EEPROM.get(EEPROM_VCCCOMPENSATION_ADDR, gv_UBat_ChipsVccCompensationValue); // this is a long, i.e. 4 bytes!
			//~ Serial.print(F("gv_UBat_ChipsVccCompensationValue from EEPROM: "));
			//~ Serial.println(gv_UBat_ChipsVccCompensationValue);
			//~ if ( gv_UBat_ChipsVccCompensationValue == -1 ) {
				//~ // nothing was stored in EEPROM --> default to assumption that Vcc is exactely 5.00V
				//~ // see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/ for details.
				//~ Serial.println(F("WARNING no ChipsVccCompensationValue found in EEPROM, defaulting to assumption that Vcc is exactely 5.00V"));
				//~ gv_UBat_ChipsVccCompensationValue = 1125300L; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
			//~ }
		//~ } // setupReadVcc()
		
		//~ /* =========================================================== */
		//~ // called by smMain at device startup + before each track + after last track + after waking up from sleep
		//~ long readVcc() {
			//~ // STEP 1: save current ADMUX settings to temporary variable
			//~ byte uBat_OldADMUX = ADMUX;
			//~ // STEP 2: set the reference to Vcc and the measurement to the internal 1.1V reference
			//~ // ATMEGA328P, others see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
			//~ ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
			//~ delay(2); // blocking wait, does not matter as we measure between tracks --> 2ms don't matter at all :-)
			//~ // STEP 3: start conversion
			//~ ADCSRA |= _BV(ADSC);
			//~ while (bit_is_set(ADCSRA,ADSC)); // bloking measuring
			//~ //  bit is reset --> conversion finished
			//~ // STEP 5: calculate Vcc
			//~ /* 		we know the measured value is     Vcc*(ADC-measure)/1023
			 //~ * 		AND we know that we can expect ADC-measure to be 1.1V = 1100mV
			 //~ * 		Solve that for Vcc to get         Vcc = 1100 * 1023 / ADC-measure
			 //~ * 		Now, ADC measure is NOT REALLY 1100mV on each individual arduino chip
			 //~ * 		therefore, we have to use a compensation value taken from EEPROM (which must be first stored there beforehand!)
			 //~ * 		gv_smUBat_ChipsVccCompensationValue: this one is individual for each chip!
			 //~ * 		(if you don't have an individually measured compensationvalue, then take the generik gv_smUBat_ChipsVccCompensationValue = 1100 * 1023 = 1125300L)
			 //~ */
			//~ byte low  = ADCL; // must read ADCL first - it then locks ADCH  
			//~ byte high = ADCH; // unlocks both
			//~ gv_UBat_in_millivolt = (high<<8) | low; // not yet compensated Vcc (in mV)!
			//~ gv_UBat_in_millivolt = gv_UBat_ChipsVccCompensationValue / gv_UBat_in_millivolt; // now its compensated!
			//~ // STEP 6: clean up
			//~ ADMUX = uBat_OldADMUX;	
		//~ } // readVcc()
 
// EOF
