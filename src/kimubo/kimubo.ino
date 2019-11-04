
#include "kimubo.h"
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine Main (smMain)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		extern State smMain_state_init(NULL, NULL, NULL);
		extern State smMain_state_idle(NULL, NULL, NULL);
		extern State smMain_state_playwav(NULL, NULL, NULL);
		//extern State smMain_state_walkietalkie(NULL, NULL, NULL);
		extern State smMain_state_status(NULL, NULL, NULL);
		extern State smMain_state_setsleeper(NULL, NULL, NULL);
		extern State smMain_state_sleeping(NULL, NULL, NULL);
		extern State smMain_state_fatalerror(NULL, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smMain_event_init_completed 						= 1;
		static const byte smMain_event_activate_wavplayer 					= 2;
		//static const byte smMain_event_activate_walkietalkie				= 3;
		static const byte smMain_event_activate_status						= 4;
		static const byte smMain_event_activate_setsleeper 					= 5;
		static const byte smMain_event_return_to_idle						= 6;
		static const byte smMain_event_return_to_wavplayer					= 7;
		//static const byte smMain_event_return_to_walkietalkie				= 8;
		static const byte smMain_event_goto_sleep 							= 9;
		static const byte smMain_event_fatalerror_detected					= 10;
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		extern Fsm smMain( &smMain_state_init );
	
	
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine Player (smWavPlayer)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smWavPlayer_state_init(NULL, NULL, NULL);
		State smWavPlayer_state_off(NULL, NULL, NULL);
		State smWavPlayer_state_stopped(NULL, NULL, NULL);
		State smWavPlayer_state_playing(NULL, NULL, NULL);
		State smWavPlayer_state_pausing(NULL, NULL, NULL);
		State smWavPlayer_state_resume_playing(NULL, NULL, NULL);
		State smWavPlayer_state_skipping(NULL, NULL, NULL);
		State smWavPlayer_state_skipping_back(NULL, NULL, NULL);
		State smWavPlayer_state_fastforwarding(NULL, NULL, NULL);
		State smWavPlayer_state_rewinding(NULL, NULL, NULL);
		State smWavPlayer_state_interrupted(NULL, NULL, NULL);
		State smWavPlayer_state_waverror(NULL, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smWavPlayer_event_init_completed 				= 1;
		static const byte smWavPlayer_event_activate		 			= 2;
		static const byte smWavPlayer_event_deactivate					= 3;
		static const byte smWavPlayer_event_play						= 4;
		static const byte smWavPlayer_event_pause				 		= 5;
		static const byte smWavPlayer_event_resume						= 6;
		static const byte smWavPlayer_event_goto_next_track				= 7;
		static const byte smWavPlayer_event_goto_previous_track			= 8;
		static const byte smWavPlayer_event_fastforward					= 9;
		static const byte smWavPlayer_event_rewind	 					= 10;
		static const byte smWavPlayer_event_cancle_current_action		= 11;
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smWavPlayer( &smWavPlayer_state_init );
 
 
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine Amplifier (smAmplifier)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
		void smAmplifier_state_init_onEnter();
		void smAmplifier_state_suspended_onEnter();
		void smAmplifier_state_on_onEnter();
		void smAmplifier_state_muted_onEnter();
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smAmplifier_state_init(&smAmplifier_state_init_onEnter, NULL, NULL);
		State smAmplifier_state_on(&smAmplifier_state_on_onEnter, NULL, NULL);
		State smAmplifier_state_muted(&smAmplifier_state_muted_onEnter, NULL, NULL);
		State smAmplifier_state_suspended(&smAmplifier_state_suspended_onEnter, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smAmplifier_event_init_completed 				= 1;
		static const byte smAmplifier_event_activate		 			= 2;
		static const byte smAmplifier_event_mute						= 3;
		static const byte smAmplifier_event_suspend						= 4;
	
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smAmplifier( &smAmplifier_state_init );
 
 
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine SDCard FET (smSdcFet)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smSdcFet_state_init(NULL, NULL, NULL);
		State smSdcFet_state_on(NULL, NULL, NULL);
		State smSdcFet_state_suspended(NULL, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smSdcFet_event_init_completed 				= 1;
		static const byte smSdcFet_event_activate		 				= 2;
		static const byte smSdcFet_event_suspend						= 3;
	
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smSdcFet( &smSdcFet_state_init );
 
 
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine Volume Poti (smVolPot)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
		/* =========================================================== */
		void smVolPot_state_init_onEnter();
		void smVolPot_state_zero_onEnter();
		void smVolPot_state_nonzero_onEnter();
		void smVolPot_state_zero_onState();
		void smVolPot_state_nonzero_onState();

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smVolPot_state_init(&smVolPot_state_init_onEnter, NULL, NULL);
		State smVolPot_state_zero(&smVolPot_state_zero_onEnter, &smVolPot_state_zero_onState, NULL);
		State smVolPot_state_nonzero(&smVolPot_state_nonzero_onEnter, &smVolPot_state_nonzero_onState, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smVolPot_event_init_completed 				= 1;
		static const byte smVolPot_event_is_zero		 				= 2;
		static const byte smVolPot_event_is_not_zero					= 3;
		
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smVolPot( &smVolPot_state_init );
 
 
	
	
 
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine Status messages (smStatus)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smStatus_state_init(NULL, NULL, NULL);
		State smStatus_state_idle(NULL, NULL, NULL);
		State smStatus_state_reporting_status(NULL, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smStatus_event_init_completed 				= 1;
		static const byte smStatus_event_report_status	 				= 2;
		static const byte smStatus_event_cancel_statusreport			= 3;
	
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smStatus( &smStatus_state_init );
 
 
	
	// ####################################################################################
	// ####################################################################################
	// ####################################################################################
	/* 
	 * Abschnitt StateMachine SleepTimer (smSleepTimer)
	 */
	 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1. state and transition callback functions
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. states
		State smSleepTimer_state_init(NULL, NULL, NULL);
		State smSleepTimer_state_idle(NULL, NULL, NULL);
		State smSleepTimer_state_timer1_active(NULL, NULL, NULL);
		State smSleepTimer_state_timer2_active(NULL, NULL, NULL);
		State smSleepTimer_state_sleeping(NULL, NULL, NULL);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. some global variables of the state machine
	
		static const long SLEEP_INTERVAL1 = 600000	;		// milliseconds between two UBat measurements
															// default is 600,000 aka 10 minutes.
		
		static const long SLEEP_INTERVAL2 = 1800000	;		// milliseconds between two UBat measurements
															// default is 1,800,000 aka 30 minutes.
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. events (values don't matter, need to be unique each within this statemachine, though)
		static const byte smSleepTimer_event_init_completed 			= 1;
		static const byte smSleepTimer_event_setsleeper_key_pressed		= 2;
		static const byte smSleepTimer_event_goto_sleep		 			= 3;
	
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. The state machine itself with its inital state
		Fsm smSleepTimer( &smSleepTimer_state_init );
	
	
	
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
	// 1. Definition KeyScanCodes
		static const byte KEYSCAN_1      = 	1	;	 
		static const byte KEYSCAN_2      = 	2	;	
		static const byte KEYSCAN_3      = 	3	;	 
		static const byte KEYSCAN_4      = 	4	;	 
		static const byte KEYSCAN_5      = 	5	;	 
		static const byte KEYSCAN_6      = 	6	;	
		static const byte KEYSCAN_7      = 	7	;	 
		static const byte KEYSCAN_8      = 	8	;	 
		static const byte KEYSCAN_9      = 	9	;	 
		static const byte KEYSCAN_FFWD   = 	10	;	 // FastForwar / Skip
		static const byte KEYSCAN_REW    = 	11	;	 // Rewind / Skipback
		static const byte KEYSCAN_STAT   = 	12	;	 // Status (output via speech)
		static const byte KEYSCAN_SLEEP  = 	13	;	 // Sleep-Timer-Set
		static const byte KEYSCAN_LOUD   = 	14	;	 // Parental switch for Volume-Preset: Loud or whisper
		static const byte KEYSCAN_PTT    = 	15	;	 // PushToTalk (for walkie talkie)
		static const byte KEYSCAN_B      = 	16	;	 // reserved for future use
					
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2. Aus wievielen Reihen und Spalten ist Matrix des Keyboards aufgebaut?

		// real hardware no. of rows/colums
		static const byte KEYB_ROWS = 4;
		static const byte KEYB_COLS = 4;
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 3. Which pin has which matrix-keyboard-row/-column?
			
		/* s. HeaderFile */ 
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 4. Which Pins are rows, which are columns 
	
		// für Power Down müssen alle Reihen-Pins auf High gesezt werden (und danach wieder auf Low)
		// dazu müssen wir festlagen, welches die Reihen-Pins und welches die Spalten-Pins sind
		static const byte KEYB_RowPins[KEYB_ROWS] = {	
			KEYB_PIN_ROW1, 
			KEYB_PIN_ROW2,
			KEYB_PIN_ROW3,
			KEYB_PIN_ROW4
		}; //connect to the row pinouts of the keypad	
										
		static const byte KEYB_ColPins[KEYB_COLS] = {
			KEYB_PIN_COLUMN1,
			KEYB_PIN_COLUMN2,
			KEYB_PIN_COLUMN3,
			KEYB_PIN_COLUMN4
		}; //connect to the column pinouts of the keypad

		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 5. damit µC zwischendurch in Powerdown gehen kann, müssen alle Keyboard-Inputs (=Reihen, s.o.) mit je einer Diode an diesen Interrupt-Pin angeschlossen sein; dieser weckt dann den µC auf)
    		
		/* s. HeaderFile */
        
        
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 6. Die TastaturBelegung selbst = welche Taste ist wo?
			
		/* s. HeaderFile */

		  
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 7. the keypad/keyboard itself		
		Keypad matrixKeyboard;


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
			Serial.print(F("smMain ChipsVccCompensationValue from EEPROM: "));
			Serial.println(v_smMain_ChipsVccCompensationValue);
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
 * Abschnitt MAIN
 */

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		bool smMain_init_stateMachine(){
			// initialize the whole statemachine, add transitions and so on...
			// transition init to idle
			smMain.add_transition(	&smMain_state_init, 
									&smMain_state_idle, 
									smMain_event_init_completed, 
									NULL);
			
			// transitions idle to player (for wav/pcm)
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_playwav,  
									smMain_event_activate_wavplayer,  
									NULL);
		    // transitions player to idle 
		    smMain.add_transition(	&smMain_state_playwav,  
									&smMain_state_idle,  
									smMain_event_return_to_idle,  
									NULL);
		    
		    // transitions idle to walkietalkie
		    //smMain.add_transition(	&smMain_state_idle,  
			//						&smMain_state_walkietalkie,  
			//						smMain_event_activate_walkietalkie,  
			//						NULL);
		    // transitions walkietalkie to idle 
		    //smMain.add_transition(	&smMain_state_walkietalkie,  
			//						&smMain_state_idle,  
			//						smMain_event_return_to_idle,  
			//						NULL);
		    
		    // transitions idle to setsleeper
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_setsleeper,  
									smMain_event_activate_setsleeper,  
									NULL);
		    // transitions setsleeper to idle 
		    smMain.add_transition(	&smMain_state_setsleeper,  
									&smMain_state_idle,  
									smMain_event_return_to_idle,  
									NULL);
		    
		    // transitions idle to sleeping
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_sleeping,  
									smMain_event_goto_sleep,  
									NULL);
		    // transitions sleeping to idle 
		    smMain.add_transition(	&smMain_state_sleeping,  
									&smMain_state_idle,  
									smMain_event_return_to_idle,  
									NULL);
		    
		    // transitions idle to status 
		    smMain.add_transition(	&smMain_state_idle,  
									&smMain_state_status,  
									smMain_event_activate_status,  
									NULL);
		    // transitions status to idle 
		    smMain.add_transition(	&smMain_state_status,  
									&smMain_state_idle,  
									smMain_event_return_to_idle,  
									NULL);
		    
		    // transitions wavplayer to setsleeper
		    smMain.add_transition(	&smMain_state_playwav,  
									&smMain_state_setsleeper,  
									smMain_event_activate_setsleeper,  
									NULL);
		    // transitions setsleeper to wavplayer 
		    smMain.add_transition(	&smMain_state_setsleeper,  
									&smMain_state_playwav,  
									smMain_event_return_to_playwav,  
									NULL);
		    
		    // transitions wavplayer to status 
		    smMain.add_transition(	&smMain_state_playwav,  
									&smMain_state_status,  
									smMain_event_activate_status,  
									NULL);
		    // transitions status to wavplayer 
		    smMain.add_transition(	&smMain_state_status,  
									&smMain_state_playwav,  
									smMain_event_return_to_playwav,  
									NULL);
		    
		    // transitions wavplayer to sleeping 
		    smMain.add_transition(	&smMain_state_playwav,  
									&smMain_state_sleeping,  
									smMain_event_goto_sleep,  
									NULL);
		    // transitions sleeping to wavplayer 
		    smMain.add_transition(	&smMain_state_sleeping,  
									&smMain_state_playwav,  
									smMain_event_return_to_playwav,  
									NULL);
		    
		    // transitions sleeping to status 
		    smMain.add_transition(	&smMain_state_sleeping,  
									&smMain_state_status,  
									smMain_event_activate_status,  
									NULL);
		    // transitions status to sleeping 
		    smMain.add_transition(	&smMain_state_status,  
									&smMain_state_sleeping,  
									smMain_event_goto_sleep,  
									NULL);
		    
		    // all is setup, now initialize the stateMachine.
		    smMain.init();
		    
		    return true; // default for now, maybe we add a more elaborate failure-management later...
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
 * Abschnitt SLEEP
 */
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// setsleeper / sleeptimer
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		bool smSleepTimer_init_stateMachine(){
			// initialize the whole statemachine, add transitions and so on...
			// transition init to idle
			smSleepTimer.add_transition(&smSleepTimer_state_init,  
										&smSleepTimer_state_idle,   
										smSleepTimer_event_init_completed,   
										NULL);
			
			// transitions idle to active1
		    smSleepTimer.add_transition(&smSleepTimer_state_idle,   
										&smSleepTimer_state_timer1_active,   
										smSleepTimer_event_setsleeper_key_pressed,   
										NULL);
			// transitions active1 to active2
		    smSleepTimer.add_transition(&smSleepTimer_state_timer1_active,   
										&smSleepTimer_state_timer2_active,   
										smSleepTimer_event_setsleeper_key_pressed,   
										NULL);
		    // transitions active2 to idle
		    smSleepTimer.add_transition(&smSleepTimer_state_timer2_active,   
										&smSleepTimer_state_idle,   
										smSleepTimer_event_setsleeper_key_pressed,   
										NULL);
		    
		    // transitions active1 to sleeping
		    smSleepTimer.add_timed_transition(	&smSleepTimer_state_timer1_active,   
												&smSleepTimer_state_sleeping,   
												SLEEP_INTERVAL1,   
												NULL);
		    // transitions active2 to sleeping
		    smSleepTimer.add_timed_transition(	&smSleepTimer_state_timer2_active,   
												&smSleepTimer_state_sleeping,   
												SLEEP_INTERVAL2,   
												NULL);
		    
		    // transitions sleeping to idle
		    smSleepTimer.add_transition(&smSleepTimer_state_sleeping,   
										&smSleepTimer_state_idle,   
										smSleepTimer_event_reset,   
										NULL);
		    
		    // all is setup, now initialize the stateMachine.
		    smSleepTimer.init();
		    
		    return true; // default for now, maybe we add a more elaborate failure-management later...
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
 * Abschnitt AMP(lifier)
 */
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// smAmplifier
	
		/* =========================================================== */
		void amplifier_suspend();{ 		digitalWrite(AMP_PIN_SUSPEND, LOW); 	}
		
		/* =========================================================== */
		void amplifier_unsuspend();{ 	digitalWrite(AMP_PIN_SUSPEND, HIGH); 	}
		
		/* =========================================================== */
		void amplifier_mute();{ 		digitalWrite(AMP_PIN_MUTE, LOW);  		}
		
		/* =========================================================== */
		void amplifier_unmute();{ 		digitalWrite(AMP_PIN_MUTE, HIGH);  		}
	
		/* =========================================================== */
		void smAmplifier_state_init_onEnter(){
			pinMode(AMP_PIN_SUSPEND,OUTPUT);
			amplifier_suspend();
			pinMode(AMP_PIN_MUTE,OUTPUT);
			amplifier_mute()
			pinMode(AMP_PIN_VOL_FEEDBACK,INPUT);
		}
	
		/* =========================================================== */
		void smAmplifier_state_suspended_onEnter(){
			amplifier_mute(); // no popping, please...
			amplifier_suspend(); 
		} // smAmplifier_state_suspended_onEnter()
		
		/* =========================================================== */
		void smAmplifier_state_on_onEnter(){
			amplifier_unsuspend();
			amplifier_unmute();
		} // smAmplifier_state_on_onEnter()
		
		/* =========================================================== */
		void smAmplifier_state_muted_onEnter(){
			amplifier_mute();
		} // smAmplifier_state_muted_onEnter()
		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		bool smAmplifier_init_stateMachine(){
			// initialize the whole statemachine, add transitions and so on...
			// transition init to suspended
			smAmplifier.add_transition(	&smAmplifier_state_init,  
										&smAmplifier_state_suspended,   
										smAmplifier_event_init_completed,   
										NULL);
			// transition suspended to on
			smAmplifier.add_transition(	&smAmplifier_state_suspended,  
										&smAmplifier_state_on,   
										smAmplifier_event_activate,   
										NULL);
			// transition on to muted
			smAmplifier.add_transition(	&smAmplifier_state_on,  
										&smAmplifier_state_muted,   
										smAmplifier_event_mute,   
										NULL);
			// transition muted to on
			smAmplifier.add_transition(	&smAmplifier_state_muted,  
										&smAmplifier_state_on,   
										smAmplifier_event_activate,   
										NULL);
										
			// all is setup, now initialize the stateMachine.
		    smAmplifier.init();
		    
		    return true; // default for now, maybe we add a more elaborate failure-management later...
		} // smAmplifier_init_stateMachine
		

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
 * Abschnitt VolumePoti
 */
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// smVolPot
	
	
		void smVolPot_measure
	
	
		/* =========================================================== */
		void smVolPot_state_init_onEnter(){
		
		}
	
		/* =========================================================== */
		void smVolPot_state_zero_onEnter(){
			// pause playing if any
			smWavPlayer.trigger(smWavPlayer_event_pause);
		}
		
		/* =========================================================== */
		void smVolPot_state_nonzero_onEnter(){
			// resume playing if any
			smWavPlayer.trigger(smWavPlayer_event_resume);
		} 
		
		/* =========================================================== */
		void smVolPot_state_zero_onState(){
			int sensorValue = analogRead(sensorPin);
			if ( sensorValue > VOLPOT_MINIMUM_LEVEL_UPPER ) {	smVolPot.trigger(smVolPot_event_is_not_zero);		}
		}
		/* =========================================================== */
		void smVolPot_state_nonzero_onState(){
			sensorValue = analogRead(sensorPin);
			if ( sensorValue <= VOLPOT_MINIMUM_LEVEL_LOWER ) {	smVolPot.trigger(smVolPot_event_is_zero);		}
		}

		
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		bool smVolPot_init_stateMachine(){
			// initialize the whole statemachine, add transitions and so on...

			smVolPot.add_transition(	&smVolPot_state_init,  
										&smVolPot_state_nonzero,   
										smVolPot_event_init_completed,   
										NULL);

			smVolPot.add_transition(	&smVolPot_state_nonzero,  
										&smVolPot_state_zero,   
										smVolPot_event_is_zero,   
										NULL);

			smVolPot.add_transition(	&smVolPot_state_zero,  
										&smVolPot_state_nonzero,   
										smVolPot_event_is_not_zero,   
										NULL);

										
			// all is setup, now initialize the stateMachine.
		    smVolPot.init();
		    
		    return true; // default for now, maybe we add a more elaborate failure-management later...
		} // smVolPot_init_stateMachine
		


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
 * Abschnitt HelferFunktionen
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
	
		// flag for failure management: if something goes wrong, this is set to FALSE. Defaults to TRUE.
		bool allIsWell = true;
	
		// setup all stateMachines
		allIsWell &= smUBat_init_stateMachine();
		allIsWell &= smMain_init_stateMachine();
		allIsWell &= smSleepTimer_init_stateMachine();
		allIsWell &= smAmplifier_init_stateMachine();
		allIsWell &= smVolPot_init_stateMachine();
	
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
	
	} // loop()	
