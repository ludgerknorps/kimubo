#include <BlockDriver.h>
#include <sdios.h>
#include <FreeStack.h>
#include <SdFat.h>
#include <SysCall.h>
#include <SdFatConfig.h>
#include <MinimumSerial.h>


#include "kimubo.h"

//adapted version of libraries!
#include "local.lib/Keypad/src/Keypad.h"
#include "local.lib/Keypad/src/Keypad.cpp"
#include "local.lib/Keypad/src/Key.h"
#include "local.lib/Keypad/src/Key.cpp"

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
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
		void smMain_state_init_onEnter();
		
		void smMain_state_idle_onEnter();
		
		void smMain_state_seek_onEnter();
		void smMain_state_seekBack_onEnter();
		
		void smMain_state_powerSave_onEnter();
		
		void smMain_state_powerSave_onExit();
		
		
		// transitionActions
		void smMain_trans_B1_onTransition();
		void smMain_trans_B2_onTransition();
		void smMain_trans_B3_onTransition();
		
		void smMain_trans_D1_onTransition();
		void smMain_trans_D2_onTransition();
		
		void smMain_trans_E1_onTransition();
		void smMain_trans_E2_onTransition();
		
		void smMain_trans_F2_onTransition();
		void smMain_trans_F5_onTransition();
		
		void smMain_trans_G1_onTransition();
		void smMain_trans_G2_onTransition();
		void smMain_trans_G4_onTransition();
		void smMain_trans_G5_onTransition();
		void smMain_trans_G6_onTransition();
		void smMain_trans_G7_onTransition();
		
		
		
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smMain_state_init(&smMain_state_init_onEnter, NULL, NULL);
		State smMain_state_idle(&smMain_state_idle_onEnter, NULL, NULL);
		State smMain_state_playWav(NULL, NULL, NULL);
		State smMain_state_pause(NULL, NULL, NULL);
		State smMain_state_seek(&smMain_state_seek_onEnter, NULL, NULL);
		State smMain_state_seekBack(&smMain_state_seekBack_onEnter, NULL, NULL);
		State smMain_state_playMessage(NULL, NULL, NULL);
		State smMain_state_powerSave(&smMain_state_powerSave_onEnter, NULL, &smMain_state_powerSave_onExit);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
		// for seeking, i.e. fast forward or backward in a track
		// 				In order to do that we play a short part of the track,
		// 				then hop to a position a little further, play again, hop again, etc.
		//				There are two relevant user-defined constants:
		//					a) SEEK_SPEEDUP				how fast shall seeking be (in times of normal play speed)
		//					b) SEEK_DURATION_PLAYING	how much of a track shall be played between hops (in milliseconds)
		#define SEEK_SPEEDUP			10
		#define SEEK_DURATION_PLAYING	10
		//				From those user-defined constants we can derive some internally relevant auto-constants:
		//					c) SEEK_INTERVAL_LENGTH		how long is a one seek interval in total (playing + hopping)
		//					d) SEEK_INTERVAL_HOP_FWD	how far do we hop forward before playing again
		//					e) SEEK_INTERVAL_HOP_BWD	how far do we hop backward before playing again
		#define SEEK_INTERVAL_LENGTH	(1000/SEEK_SPEEDUP)
		#define SEEK_INTERVAL_HOP_FWD	(SEEK_INTERVAL_LENGTH-SEEK_DURATION_PLAYING)
		#define SEEK_INTERVAL_HOP_BWD	(SEEK_INTERVAL_LENGTH+SEEK_DURATION_PLAYING)
		
		// for powersaving there is one relevant user-defined constant:
		//					a) POWERSAVE_AFTER			after which time (in milliseconds) shall device enter low-power-state
		//												coming from idle, the low-power-state is recoverable, 
		//												i.e. we can return to idle on e.g. keypress
		#define POWERSAVE_AFTER			30000
		//				From this user-defined constant we can derive some internally relevant auto-constants:
		//					b) POWERSAVE_AFTER_PAUSE	after which time in pause-state shall device enter low-power-state
		//												(in times of POWERSAVE_AFTER, not milliseconds!)
		//												coming from pause, the low-power-state is recoverable, 
		// 												i.e. we can return to pause on e.g. keypress BUT NOT by just turning
		//												the volume up again, thus do not choose a too small value here!
		#define POWERSAVE_AFTER_PAUSE	(60*POWERSAVE_AFTER)
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		#define smMain_event_init_completed 				1
		#define smMain_event_stop			 				5
		#define smMain_event_playWav			 			10
		#define smMain_event_playNextWav					12
		#define smMain_event_playOtherPlaylist				13
		#define smMain_event_pause							15
		#define smMain_event_resume							20
		#define smMain_event_skip				 			25
		#define smMain_event_skipBack						30
		#define smMain_event_seek							35
		#define smMain_event_seekBack						40
		#define smMain_event_gotoSleep 						45
		#define smMain_event_wakeUpToIdle					47
		#define smMain_event_wakeUpToPause					48
		#define smMain_event_playMessage					50
		#define smMain_event_playNextMessagePart			53
		#define smMain_event_continue						55
		#define smMain_event_returnToPause					60
		#define smMain_event_returnToIdle					65
		#define smMain_event_fatalerror_detected			255
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. init function for smMain
		bool smMain_init_stateMachine(){
			// initialize the whole statemachine, add transitions and so on...
			
			// A1 transition init to idle
			// we are ready but not doing anything yet
			smMain.add_transition(	&smMain_state_init, 
									&smMain_state_idle, 
									smMain_event_init_completed, 
									NULL);
									
			// --------- PLAYING / play a track and at end of track continue with next track ---------
			
			// B1 transitions idle to playWav (normal player, not messages)
			// start playing something
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_playWav,  
									smMain_event_playWav,  
									&smMain_trans_B1_onTransition);
			
			// B2 transitions playWav to playWav
			// we have finished to play something and we now want to continue with next track
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_playWav,  
									smMain_event_playNextWav,  
									&smMain_trans_B2_onTransition);
									
			// B3 transitions playWav to playWav
			// we are playing and user wants to play other playlist
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_playWav,  
									smMain_event_playOtherPlaylist,  
									&smMain_trans_B3_onTransition);
									
			// --------- STOPPING / stop to play ---------
									
			// C1 transitions playWav to idle
			// we are playing or have finished to play and now want to stop playing
			// this transition is also used by sleepTimer (in order to end playing and later go to low power mode)
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_idle,  
									smMain_event_stop,  
									NULL);
									
			// C2 transitions pause to idle
			// we have paused and now want to stop playing
			// this transition is ONLY used by sleepTimer (in order to end playing and later go to low power mode)
		    smMain.add_transition(	&smMain_state_pause,  
									&smMain_state_idle,  
									smMain_event_stop,  
									NULL);
			
			// --------- PAUSING / halt playing and later continue playing seamlessless --------------
			
			// D1 transitions playWav to pause
			// we are playing something and want to pause
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_pause,  
									smMain_event_pause,  
									&smMain_trans_D1_onTransition);
								
			// D2 transitions pause to playWav
			// we have paused and now want to resume (aka. continue to play)
		    smMain.add_transition(	&smMain_state_pause,  
									&smMain_state_playWav,  
									smMain_event_resume,  
									&smMain_trans_D2_onTransition);
									
			// no extra transition for events 
			// 						smMain_event_playOtherPlaylist
			//						smMain_event_skip
			//						smMain_event_skipBack
			// 						smMain_event_seek
			//						smMain_event_seekBack
			// if user presses one of those buttons in pause, nothing happens at all!
									
			// -------- SKIPPING / Going to next or previous track by pressing buttons ---------------
									
			// E1 transitions playWav to playWav
			// we are playing something and want to skip (to next track)
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_playWav,  
									smMain_event_skip,  
									&smMain_trans_E1_onTransition);											
									
		    // E2 transitions playWav to playWav
			// we are playing something and want to skipBack (to previous track)
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_playWav,  
									smMain_event_skipBack,  
									&smMain_trans_E2_onTransition);	
			
			// --------- SEEKING / Going fast-forward or fast-backward in track -----------------------
									
			// F1 transitions playWav to seek
			// we are playing something and want to seek (forward)
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_seek,  
									smMain_event_seek,  
									NULL);	
			
			// F2 transitions seek to seek
			// hop forward while seeking
		    smMain.add_timed_transition(	&smMain_state_seek,  
											&smMain_state_seek,  
											SEEK_DURATION_PLAYING,  
											&smMain_trans_F2_onTransition);	
									
			// F3 transitions seek to playWav
			// we were seeking and now return to play
		    smMain.add_transition(	&smMain_state_seek,  
									&smMain_state_playWav,  
									smMain_event_continue,  
									NULL);	
			
			// F4 transitions playWav to seekBack
			// we are playing something and want to seek (backward)
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_seekBack,  
									smMain_event_seekBack,  
									NULL);
			
			// F5 transitions seek to seek
			// hop backward while seeking backwards
		    smMain.add_timed_transition(	&smMain_state_seek,  
											&smMain_state_seek,  
											SEEK_DURATION_PLAYING,  
											&smMain_trans_F5_onTransition);	
												
			// F6 transitions seekBack to playWav
			// we were seeking and now return to play
		    smMain.add_transition(	&smMain_state_seekBack,  
									&smMain_state_playWav,  
									smMain_event_continue,  
									NULL);
			
			// --------- PLAYMESSAGES / play a message and if necessary interrupt playing normal wav for that --------------
			
			// G1 transitions idle to playMessage
			// we are not doing anything and now and want to play a message
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_playMessage,  
									smMain_event_playMessage,  
									&smMain_trans_G1_onTransition);	
									
			// G2 transitions playMessage to playMessage
			// we are playing a message, finished with one part of the message and now want to play the next part
		    smMain.add_transition(	&smMain_state_playMessage,  
									&smMain_state_playMessage,  
									smMain_event_playNextMessagePart,  
									&smMain_trans_G2_onTransition);						
			
			// G3 transitions playMessage to idle
			// we finished playing a message and return to do nothing
		    smMain.add_transition(	&smMain_state_playMessage,  
									&smMain_state_idle,  
									smMain_event_returnToIdle,  
									NULL);	
			
			// G4 transitions playWav to playMessage
			// we are playing a normal wav and now want to interupt that for a message
		    smMain.add_transition(	&smMain_state_playWav,  
									&smMain_state_playMessage,  
									smMain_event_playMessage,  
									&smMain_trans_G4_onTransition);						
			
			// G5 transitions playMessage to playWav
			// we finished playing a message and return to playing normally
		    smMain.add_transition(	&smMain_state_playMessage,  
									&smMain_state_playWav,  
									smMain_event_continue,  
									&smMain_trans_G5_onTransition);	
									
			// G6 transitions pause to playMessage
			// we are pausing a normal wav and now want to interupt that for a message
		    smMain.add_transition(	&smMain_state_pause,  
									&smMain_state_playMessage,  
									smMain_event_playMessage,  
									&smMain_trans_G6_onTransition);						
			
			// G7 transitions playMessage to pause
			// we finished playing a message and return to pausing 
		    smMain.add_transition(	&smMain_state_playMessage,  
									&smMain_state_pause,  
									smMain_event_returnToPause,  
									&smMain_trans_G7_onTransition);						
			
			// --------- POWERSAVE / send device to low power mode and wake up again --------------
			
			// H1 transitions idle to powerSave
			// we are doing nothing and now want to save power
		    smMain.add_timed_transition(	&smMain_state_idle,  
											&smMain_state_powerSave,  
											POWERSAVE_AFTER,  
											NULL);
											
			// H2 transitions powerSave to idle
			// recover from low power state
		    smMain.add_transition(	&smMain_state_powerSave,  
									&smMain_state_idle,  
									smMain_event_wakeUpToIdle,  
									NULL);	
			
			// H3 transitions pause to powerSave
			// we are pausing (for quite some time!) and now want to save power
		    smMain.add_timed_transition(	&smMain_state_pause,  
											&smMain_state_powerSave,  
											POWERSAVE_AFTER_PAUSE,  
											NULL);	
			
			// H4 transitions powerSave to pause
			// recover from low power state
		    smMain.add_transition(	&smMain_state_powerSave,  
									&smMain_state_pause,  
									smMain_event_wakeUpToPause,  
									NULL);
									
			// no need for a transition from playWav to powerSave:
			// sleepTimer does not directly set powerSave-state but instead
			// stops playing - after which the normal H1 transition is used to 
			// put device into low power state after a certain time...
														
		} // smMain_init_stateMachine()
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. long definition of all callback functions
		
		/* =========================================================== */
		// called once at device startup
		void smMain_state_init_onEnter(){
			
			// -------------------------------------------------------------------
			// Setup Timers for PCM Output
			// -------------------------------------------------------------------
				//ludgerknorps ISR-debug
				#if defined (ISR_BUFFER_DEBUG_PIN)
					pinMode(ISR_BUFFER_DEBUG_PIN,OUTPUT);
					setPinD0_D21(ISR_BUFFER_DEBUG_PIN);
				#endif
				#if defined (ISR_PCMFEED_DEBUG_PIN)
					pinMode(ISR_PCMFEED_DEBUG_PIN,OUTPUT);
					setPinD0_D21(ISR_PCMFEED_DEBUG_PIN);
				#endif
				
				//Reset Timer/Counter Control Register A and B
				TCCR1A = 0;
				TCCR1B = 0;
				
				//Modus Phase+FrequCorrect PWM-Mode TOP = ICR1 (Mode 8), see [ATMEGA328 datasheet DS40002061A-page 141]
				TCCR1A &= ~_BV(WGM10);
				TCCR1A &= ~_BV(WGM11);
				TCCR1B &= ~_BV(WGM12);
				TCCR1B |=  _BV(WGM13);
				
				// TOP = ICR1 is set later by metadata read from PCM-file...
				//~ ICR1 = ...;	
				
				//Timer1 prescaler: set to 1
				TCCR1B &= ~_BV(CS12);
				TCCR1B &= ~_BV(CS11);
				TCCR1B |=  _BV(CS10);

				//noninverting PWM-Signal for channels A and B
				TCCR1A |=  _BV(COM1A1);
				TCCR1A &= ~_BV(COM1A0);
				TCCR1A |=  _BV(COM1B1);
				TCCR1A &= ~_BV(COM1B0);
			// End of Setup Timers for PCM Output
			
			// -------------------------------------------------------------------
			// define and set PWM pins
			// -------------------------------------------------------------------
			
				// PWM pins pair MSB=9 LSB=10, together 16bit PWM :-)
				// configure both PWM-output pins (they are defined in .h file)
				pinMode(AUDIO_PIN_OUT_HIGH,OUTPUT);
				pinMode(AUDIO_PIN_OUT_LOW,OUTPUT);

				#if defined (SD_FULLSPEED)
					SPSR |= (1 << SPI2X);
					SPCR &= ~((1 <<SPR1) | (1 << SPR0));
				#endif
				SD.begin(SD_ChipSelectPin)
		}
		
		
		/* =========================================================== */
		void smMain_state_idle_onEnter(){
			// tbd.
				LKpcm::stopPlayback()
		}
		
		/* =========================================================== */
		void smMain_state_seek_onEnter(){
			// tbd.
				LKpcm::play(char* filename, unsigned long seekPoint)
		}
		
		/* =========================================================== */
		void smMain_state_seekBack_onEnter(){
			// tbd.
				LKpcm::play(char* filename, unsigned long seekPoint)
		}
		
		
		
		
		
		/* =========================================================== */
		void smMain_trans_B1_onTransition(){
			// tbd.
				LKpcm::play(char* filename)
		}
		
		/* =========================================================== */
		void smMain_trans_B2_onTransition(){
			// tbd.
				LKpcm::play(char* filename)
		}
		
		/* =========================================================== */
		void smMain_trans_B3_onTransition(){
			// tbd.
				LKpcm::play(char* filename)
		}
		
		
		/* =========================================================== */
		void smMain_trans_D1_onTransition(){
			// tbd.
				LKpcm::pause()
		}
		
		/* =========================================================== */
		void smMain_trans_D2_onTransition(){
			// tbd.
				LKpcm::pause()
		}
		
		
	
	
		/* =========================================================== */
		void smMain_trans_B1_onTransition(){
			// tbd.
				LKpcm::play(char* filename)
		}
		
		
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
		
		// User defined constants	
			#define NUMBER_OF_PCM_BUFFERS 	4  	
			// how many pcm buffers, do we use? 
			// each can be max 256 bytes big (because it is addressed by a byte-type). 
			// buffer size must be even number.
			// all together must fit into RAM. 
			// All buffers together form one big "semi-ring-buffer".
			#define PCM_BUFFER_SIZE 		128  // must be even number (because of 16bit == 2 byte read at-a-time)	 
		
		
		// lobal public functions (because of ISR we cannot put them easily into a class!)
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
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 0. a semi-statemachine for the keys
	
		static bool keybIsMoreThanOneKeyPressed = false; // true, if more than one key is pressed - we don't want multikey right now!
		static char keybPressedKey = NULL; 	// is set to a key that is pressed alone. Used to act only at release of this key, not on pressing it. Also used for acting on hold of key.
		static char keybHeldKey = NULL; 		//used to remember which key was held (to act on its release)
		

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. Definition KeyScanCodes
    // see .h file
					
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. Aus wievielen Reihen und Spalten ist Matrix des Keyboards aufgebaut?
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. Which pin has which matrix-keyboard-row/-column?
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. Which Pins are rows, which are columns 
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. damit µC zwischendurch in Powerdown gehen kann, müssen alle Keyboard-Inputs (=Reihen, s.o.) mit je einer Diode an diesen Interrupt-Pin angeschlossen sein; dieser weckt dann den µC auf)
    // see .h file
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. Die TastaturBelegung selbst = welche Taste ist wo?
    // see .h file
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 7. keypad settings
    // see .h file
		  
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 8. the keypad/keyboard itself		
		extern Keypad keypad; // defined in setup()
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	// 9. the eventListener callback function for different types of keys
	
		void keypadEvent(KeypadEvent key){
			// if more than one key is pressed, then
			// don't do anything with pressed keys
			// don't react on released keys
			
			// even better way: redefine LIST_MAX to be 1
			// then only one key can be added to keys-list. :-)
			
			if ( keypad.numKeys() > 1 ) {
				keybPressedKey = NULL; // if several keys are pressed together, don't do anything, thus forget pressed/held keys and clear all keys from keylist.
				keybHeldKey = NULL;
				keypad.clearList();
			} else {

				switch (keypad.getState()){
					case PRESSED:
						keybPressedKey = key;
						break;
						
					case HOLD:
						keybHeldKey = key;
						if ( key ='F' ) {
							// seeking while button is held
							
						} else if ( key ='R') {
							// seeking back while button is held
							
						}
						break;

					case RELEASED:
						// Handle errors:
						// 1. case: 
							if (key != keybPressedKey) {
								// this must never happen : how should keybPressedKey != key, if only one key is allowed AND one key was released?
								break;
							}
						// 2. case:
							if ( (keybHeldKey != NULL) && (key != keybHeldKey) ) {
								// this must never happen: how should keybHeldKey != key, if only one key is allowed AND one key was released?
								break;
							}
						// if we came to here, then no errors apparent
					
						if (key >= '1' && key <='9') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// play playlist nr. <key> from beginning
							} else {
								// key released after held
								// play playlist nr. <key> from stored position (from EEPROM)
							} 
						} else if (key=='F') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to next track in playlist
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='B') {
							if (keybHeldKey == NULL) {
								// key released after pressed but not held
								// skip to previous track in playlist
							} else if (key == keybHeldKey) {
								// key released after held
								// we were seeking (keystate was HOLD) --> now in RELEASE we don't skip/do anything else
							}
						} else if (key=='S') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setSleeper()
						} else if (key=='L') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// setLoudness()
						} else if (key=='Z') {
							// key released after pressed, doesn't matter if held or not (because key is not used doubly)
							// tellStatus()
						}
						
						// clean up the flags (remember: only one key allowed at any time!)
						keybPressedKey = NULL;
						keybHeldKey = NULL;
						break;
				} // switch
			} // if
		} // keypadEvent
		

		
		


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
 * Abschnitt PowerManagement
 */
 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Read 1.1V reference against AVcc
	// taken from http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
	// 
	// blocking does not matter to us because we measure between tracks -->  some ms don't matter at all :-)
	
		/* some globals */
		
		long gv_UBat_ChipsVccCompensationValue;
		long gv_UBat_in_millivolt;
		
		/* =========================================================== */
		// called once at device startup
		void setupReadVcc(){
			// STEP 0: Read gv_smUBat_ChipsVccCompensationValue from EEPROM (see below for explanation on that value!)
			EEPROM.get(EEPROM_VCCCOMPENSATION_ADDR, gv_UBat_ChipsVccCompensationValue); // this is a long, i.e. 4 bytes!
			Serial.print(F("gv_UBat_ChipsVccCompensationValue from EEPROM: "));
			Serial.println(gv_UBat_ChipsVccCompensationValue);
			if ( gv_UBat_ChipsVccCompensationValue == -1 ) {
				// nothing was stored in EEPROM --> default to assumption that Vcc is exactely 5.00V
				// see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/ for details.
				Serial.println(F("WARNING no ChipsVccCompensationValue found in EEPROM, defaulting to assumption that Vcc is exactely 5.00V"));
				gv_UBat_ChipsVccCompensationValue = 1125300L; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
			}
		} // setupReadVcc()
		
		/* =========================================================== */
		// called by smMain at device startup + before each track + after last track + after waking up from sleep
		long readVcc() {
			// STEP 1: save current ADMUX settings to temporary variable
			byte uBat_OldADMUX = ADMUX;
			// STEP 2: set the reference to Vcc and the measurement to the internal 1.1V reference
			// ATMEGA328P, others see http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
			ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
			delay(2); // blocking wait, does not matter as we measure between tracks --> 2ms don't matter at all :-)
			// STEP 3: start conversion
			ADCSRA |= _BV(ADSC);
			while (bit_is_set(ADCSRA,ADSC)); // bloking measuring
			//  bit is reset --> conversion finished
			// STEP 5: calculate Vcc
			/* 		we know the measured value is     Vcc*(ADC-measure)/1023
			 * 		AND we know that we can expect ADC-measure to be 1.1V = 1100mV
			 * 		Solve that for Vcc to get         Vcc = 1100 * 1023 / ADC-measure
			 * 		Now, ADC measure is NOT REALLY 1100mV on each individual arduino chip
			 * 		therefore, we have to use a compensation value taken from EEPROM (which must be first stored there beforehand!)
			 * 		gv_smUBat_ChipsVccCompensationValue: this one is individual for each chip!
			 * 		(if you don't have an individually measured compensationvalue, then take the generik gv_smUBat_ChipsVccCompensationValue = 1100 * 1023 = 1125300L)
			 */
			byte low  = ADCL; // must read ADCL first - it then locks ADCH  
			byte high = ADCH; // unlocks both
			gv_UBat_in_millivolt = (high<<8) | low; // not yet compensated Vcc (in mV)!
			gv_UBat_in_millivolt = gv_UBat_ChipsVccCompensationValue / gv_UBat_in_millivolt; // now its compensated!
			// STEP 6: clean up
			ADMUX = uBat_OldADMUX;	
		} // readVcc()
		
		
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
/* 
 * We have certain user interactions (setSleepTimer, readStatus, setLoudness) that require a feedback from the 
 * KIMUBO to the user (in this case: parental user).
 * Feedback is provided by the KIMUBO "speaking" to the user via pre-fabricated PCM files ("message-files").
 * Those are also stored on the SDC in directory "/KIMUBOsystemmessages".
 * 
 * Prefabricated message-files have filenames 
 * 			001.WAV, 002.WAV, 003.WAV, ..., 255.WAV
 * We will be able to handle 255 different such files - addressing is done via a byte-type variable.
 * 
 * Playing messages is possible if either
 * 		a) no normal WAV is being played (== player is stopped)
 * 		b) normal WAV is being played (== player gets interrupted and remembers current position in normal WAV, message is played, normal WAV continues seamlessly)
 * 
 * Thus, 
 * 		REQ1) message-player needs to be able to interrupt normal player and 
 * 		REQ2) normal player needs to be 
 * 			a) able to be interrupted and
 * 			b) remember current position and 
 * 			c) continue from that onwards.
 * 
 * Messages consist of several messge-tokens, each token to be stored inside a specific message-file.
 * E.g. if we want to read out the current BAT voltage, then we might want to say:
 * 		"Batteriespannung beträgt 4,8V"		("battery voltage is 4.8V")
 * This would translate to tokens
 * 		TOKEN	MESSAGE (german)					MESSAGE (engl. transl.)		BYTEVALUE (e.g.)
 * 	  ----------------------------------------------------------------------------------------
 * 		TOKEN1	"Batteriespannung beträgt "			("battery voltage is ")		17
 * 		TOKEN2	"vier"								("four")					4
 * 		TOKEN3	"Komma"								("point")					11
 * 		TOKEN4	"acht"								("eight")					8
 * 		TOKEN5	"Volt"								("volts")					18
 * 
 * These tokens are represented by a specific byte-value each.
 * 
 * If we want to define this message we need a byte[] = [17, 4, 11, 8, 18].
 * If we want to play that message we need to
 * 		- (if necessary) interupt current normal playing
 * 		- iterate over our byte[] and play the corresponding message-files
 * 		- (if necessary) continue with normal playing
 * 
 * In order to find the correct message-file from its corresponding byte-value a conversion is necessary: getMessageFilenameFromByteValue(byte byteValue)
 */
		
		//  global variable, we only need one filename at a time
		static char messageFileName[8];  

		char* getMessageFilenameFromByteValue(byte byteValue){
			
			// we overwrite the char-array at address messageFileName, thus we need a pointer to that address which we can manipulate
			char* p = messageFileName;
			
			// all message-files have a fixed filename-length of xxx.yyy, 
			//     where xxx is the corresponding byte value (as a String withlength 3) and 
			//     .yyy is the globally defined SUFFIX_PCM_FILES (typically something like ".WAV")
			if (byteValue < 10) {
				p = '0';					// write '0' to address messageFileName			
				p++;
				p = '0';					// write another '0' in next position  ( messageFileName now is "00xxxxxx" where x can be any char including '\0')
				p++;
				itoa(byteValue, p, 10); 	// write the single digit into next position ( messageFileName now is "00DTxxxx" where D is digit, and T='\0', and x can be any char including '\0')
			} else if (byteValue < 100) {
				p = '0';					// write '0' to address messageFileName		
				p++;
				itoa(byteValue, p, 10); 	// write two digits into next positions ( messageFileName now is "0DDTxxxx" where D is digit, and T='\0', and x can be any char including '\0')
			} else {
				itoa(byteValue, p, 10); 	// write three digits into next positions ( messageFileName now is "DDDTxxxx" where D is digit, and T='\0', and x can be any char including '\0')
			}
			
			// now add file-suffix 
			strncat(messageFileName, SUFFIX_PCM_FILES, 8); 
			// messageFileName now is "DDD.WAVT" where D is digit, ".WAV" is just that and T='\0'
			return messageFileName;
			
		}


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

  Keypad keypad = Keypad( makeKeymap(KEYB_keyScans), KEYB_RowPins, KEYB_ColPins, KEYB_ROWS, KEYB_COLS );
  
  void setup() {
  
    // Setup of Keypad/keyboard
    
    keypad.setHoldTime(KEYB_HOLD_TIME);
    keypad.setDebounceTime(KEYB_DEBOUNCE_TIME);
    // add an event listener 
    keypad.addEventListener(keypadEvent); 
  
  } // setup()


 

	
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
	
	void loop() {
		
		// check if something is happening on the keypad (e.g. key pressed released)
		keypad.getKeys(); // we only need to check for one key as we do not use multikey - but the whole eventListener-thing in keypad works with "getKeys()".
		// tbd. define MAX_KEYS 1!
	
	} // loop()	
