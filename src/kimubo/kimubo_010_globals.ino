
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
