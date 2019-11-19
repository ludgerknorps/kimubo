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
 * Abschnitt fast (!) replacements for digitalWrite() and digitalRead()
 */
	
	// ludgerknorps stuff
	// fast (!) replacements for digitalWrite() and digitalRead() - about 20x faster than digitalWrite().
	// see https://www.best-microcontroller-projects.com/arduino-digitalwrite.html for more explanation
	// use as in 
	// 			setPinD0_D7(5)			--> set Pin D5  aka. PORTD.5 = HIGH
	//			clrPinD8_D13(12)		--> clear Pin D12 aka. PORTB.5 = LOW
	// 			readPinA0_A7(5)			--> read Pin A5  aka. PORTC.5 
	//			writePinD0_D7(5,LOW)	--> clear Pin D5  aka. PORTD.5 = LOW 
		#define setPinD0_D7(pinNo) 		( PORTD |=(1<<(pinNo)) )
		#define setPinD8_D13(pinNo) 	( PORTB |=(1<<(pinNo-8)) )
		#define setPinA0_A7(pinNo) 		( PORTC |=(1<<(pinNo)) )
		#define setPinD14_D21(pinNo) 	( setPinA0_A7(pinNo-13) )

		#define clrPinD0_D7(pinNo) 		( PORTD &=~(1<<(pinNo)) )
		#define clrPinD8_D13(pinNo) 	( PORTB &=~(1<<(pinNo-8)) )
		#define clrPinA0_A7(pinNo) 		( PORTC &=~(1<<(pinNo)) )
		#define clrPinD14_D21(pinNo) 	( clrPinA0_A7(pinNo-13) )

		// more comfortable but slightly slower 
		#define writePinD0_D7(pinNo, boolValue) 	( (boolValue && setPinD0_D7(pinNo))  || clrPinD0_D7(pinNo) )
		#define writePinD8_D13(pinNo, boolValue) 	( (boolValue && setPinD8_D13(pinNo)) || clrPinDD8_D13(pinNo) )
		#define writePinA0_A7(pinNo, boolValue) 	( (boolValue && setPinA0_A7(pinNo))  || clrPinA0_A7(pinNo) )
		
		// more comfortable but even more (slightly) slower 
		#define setPinD0_D21(pinNo) 				( ((pinNo)>13) ? setPinA0_A7(pinNo-13) : ( (pinNo)>8 ? setPinD8_D13(pinNo-8) : setPinD0_D7(pinNo) ) )
		#define clrPinD0_D21(pinNo) 				( ((pinNo)>13) ? clrPinA0_A7(pinNo-13) : ( (pinNo)>8 ? clrPinD8_D13(pinNo-8) : clrPinD0_D7(pinNo) ) )
		#define writePinD0_D21(pinNo, boolValue) 	( (boolValue && setPinD0_D21(pinNo))  || clrPinD0_D21(pinNo) )
		
		// sometimes reading is better than writing...
		#define readPinD0_D7(pinNo) 	( (PORTD &(1<<(pinNo)))!=0 )
		#define readPinD8_D13(pinNo) 	( (PORTB &(1<<(pinNo-8)))!=0 )
		#define readPinA0_A7(pinNo) 	( (PORTD &(1<<(pinNo)))!=0 )






/* 
 * Abschnitt Audio / PCM Player
 */


		// global public functions (because of ISR we cannot put them easily into a class!)
			bool isPlaying();
			bool isPaused();
			bool isStopped();
			bool isInterupted();
			bool isTwoByteSamples();
			bool is16bit();
			
		//*********** Global Variables of PCM player ***************
			volatile boolean buffEmpty[NUMBER_OF_PCM_BUFFERS];
			volatile byte currentReadBuffer = 0;
			volatile byte currentWriteBuffer = 0;

			//*** Options from MSb to LSb: 7=paused, 6=stopped, 5=interupted, 4=playing, 3=N.N., 2=2-byte samples, 1=16-bit ***
			volatile byte playerOptions = B00000000; 

			volatile byte buffer[NUMBER_OF_PCM_BUFFERS][PCM_BUFFER_SIZE];
			volatile byte buffCount = 0;
			char volMod=0;
			
						
			#if !defined (SDFAT)
				File sFile;
			#else
				SdFile sFile;
			#endif
			
		// Helper-functions (not used in ISR for performance!)
		
			// we want to 
			// 1) use several helper-functions in order to clean up the code and
			// 2) on the other side don't want to use function calls inside of ISR (wherever possible) because of performance impacts and
			// 3) we do want to code behaviour only oncc.
			// Thus - allthough is is not optimal coding style - we declare macros, use them inside of functions and inside ISR as well

			#define ENABLE_BUFFER_INTERUPT			(TIMSK1 |= _BV(ICIE1))
			#define DISABLE_BUFFER_INTERUPT			(TIMSK1 &= ~_BV(ICIE1))
			#define ENABLE_PCM_FEED_INTERUPT		(TIMSK1 |= _BV(TOIE1))
			#define DISABLE_PCM_FEED_INTERUPT		(TIMSK1 &= ~_BV(TOIE1))

			void enableBufferInterupt(){
				ENABLE_BUFFER_INTERUPT;
			}
			void disableBufferInterupt(){
				DISABLE_BUFFER_INTERUPT;
			}

			void enablePCMFeedInterupt(){
				ENABLE_PCM_FEED_INTERUPT;
			}
			void disablePCMFeedInterupt(){
				DISABLE_PCM_FEED_INTERUPT;
			}

			bool isPaused(){
				return bitRead(playerOptions,7);
			}

			bool isStopped(){
				return bitRead(playerOptions,6);
			}

			bool isInterupted(){
				return bitRead(playerOptions,7);
			}
			
			bool isPlaying(){
				return bitRead(playerOptions,4);
			}

			bool isTwoByteSamples() {
				return bitRead(playerOptions,2);
			}

			bool is16bit() {
				return bitRead(playerOptions,1);
			}

			void setPaused(bool bParam) {
				bitWrite(playerOptions,7,bParam);
			}

			void setStopped(bool bParam) {
				bitWrite(playerOptions,6,bParam);
			}

			void setInterupted(bool bParam) {
				bitWrite(playerOptions,5,bParam);
			}
			
			void setPlaying(bool bParam) {
				bitWrite(playerOptions,4,bParam);
			}

			void setTwoByteSamples(bool bParam) {
				bitWrite(playerOptions,2,bParam);
			}

			void set16bit(bool bParam) {
				bitWrite(playerOptions,1,bParam);
			}







/* 
 * Abschnitt UBat measurement
 */




		long gv_UBat_ChipsVccCompensationValue;
		long gv_UBat_in_millivolt; 







// EOF
