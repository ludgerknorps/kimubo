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
