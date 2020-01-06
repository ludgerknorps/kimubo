// write a compensation value to KIMUBO arduino pro mini

// put the right base address here (the first byte of the four byte long-variable in EEPROM)
// this MUST match the value given in kimubo.h!
	static const byte EEPROM_VCCCOMPENSATION_ADDR	=	0; 

/* 		we know the measured value is     Vcc*(ADC-measure)/1023
 * 		AND we know that we can expect ADC-measure to be 1.1V = 1100mV
 * 		Solve that for Vcc to get         Vcc = 1100 * 1023 / ADC-measure
 * 		Now, ADC measure is NOT REALLY 1100mV on each individual arduino chip
 * 		therefore, we have to use a compensation value taken from EEPROM (which must be first stored there beforehand!)
 * 		gv_smUBat_ChipsVccCompensationValue: this one is individual for each chip!
 * 		(if you don't have an individually measured compensationvalue, then take the generic gv_smUBat_ChipsVccCompensationValue = 1100 * 1023 = 1125300L)
 * 		
 * 		In order to measure this value do the following:
 * 		a) let readVcc() run with default value 1125300L
 * 		b) measure with multimeter the real Vcc
 * 		c) calculate gv_smUBat_ChipsVccCompensationValue = 1.1 * Vcc_of_multimeter / Vcc_of_readVcc_function * 1023 * 1000
 * 		d) write that value gv_smUBat_ChipsVccCompensationValue to EEPROM to the same address you will read it back from at startup
 * 			use e.g. 
 */
	long  gv_smUBat_ChipsVccCompensationValue = 1117936L;
	long  readBack = 0L;






#include <EEPROM.h>

void setup() {

  Serial.begin(38400);

  //One simple call, with the address first and the object second.
  EEPROM.put(EEPROM_VCCCOMPENSATION_ADDR, gv_smUBat_ChipsVccCompensationValue);

  Serial.print("Written float data type! Now checking by reading back... ");

  EEPROM.get(EEPROM_VCCCOMPENSATION_ADDR, readBack);
  Serial.println(readBack);  
}

void loop() {
  /* Empty loop */
}
