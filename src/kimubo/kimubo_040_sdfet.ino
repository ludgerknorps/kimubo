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
 
bool sdc_setup(){
    //SDFAT
    //if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    // Initialize at the highest speed supported by the board that is
    // not over 50 MHz. Try a lower speed if SPI errors occur.
    if (!sd.begin(SDC_PIN_CS, SD_SCK_MHZ(50))) {
        #if defined (debug)
            Serial.println(F("SD fail _ _ _"));  
        #endif
        return false;   // don't do anything more if not
    
    }
    else{   
        #if defined (debug)
            Serial.println(F("SD ok"));
        #endif   
    }

    return true;
  
}



//bool sdc_setup_old(){
//
//    #if defined (debug)
//        Serial.println(F("sdc_setup INFO started..."));
//    #endif
//
//    char c; // temporary var.
//    byte i; // temporary var.
//    byte j; // temporary var.
//    unsigned int  fileNumber;
//
//    FRESULT result; // from PetitFS
//    DIR directory; // dito
//    FILINFO fileinfo; // dito
//    char dirpath[2]; // initialize as empty string
//    byte filenamelength;
//  
//  	// Initialize SDcard
//  	// Try a lower speed if SPI errors occur.
//    result = pf_mount(&sdc_fileSystem);
//  	switch ( result ){
//        case FR_OK:
//            // do nothing...
//            break;
//            
//        case FR_NOT_READY:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_mount failed with FR_NOT_READY"));
//            #endif
//            return false;
//
//        case FR_DISK_ERR:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_mount failed with FR_DISK_ERR"));
//            #endif
//            return false;
//
//        case FR_NO_FILESYSTEM:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_mount failed with FR_NO_FILESYSTEM"));
//            #endif
//            return false;
//      
//  	} // switch
//  
//  
//  	// in our root directory there must exist subdirs '0' to '8' and a subdir SDC_SYSTEM_MESSAGES_DIR
//    // there must not exist something else!
//    
//    // scan through root dir...
//    result = pf_opendir( &directory, "/" );
//    switch ( result ){
//    
//        case FR_OK:
//            // do nothing...
//            break;
//            
//        case FR_NO_FILE:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_opendir / failed with FR_NO_FILE"));
//            #endif
//            return false;
//            
//        case FR_NOT_READY:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_opendir / failed with FR_NOT_READY"));
//            #endif
//            return false;
//            
//        case FR_DISK_ERR:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_opendir / failed with FR_DISK_ERR"));
//            #endif
//            return false;
//            
//        case FR_NOT_ENABLED:
//            #if defined (debug)
//                Serial.println(F("sdc_setup ERROR pf_opendir / failed with FR_NOT_ENABLED"));
//            #endif
//            return false;
//    } // switch 
//    
//  	// repeatedly call readdir in order to scan through root directory
//    i = 0; // this is our counter to check that we have the right number of items in root dir
//    for (;;) {
//        result = pf_readdir(&directory, &fileinfo);
//        switch ( result ){
//    
//            case FR_OK:
//                // do nothing...
//                break;
//                
//            case FR_DISK_ERR:
//                #if defined (debug)
//                    Serial.println(F("sdc_setup ERROR pf_readdir failed with FR_DISK_ERR"));
//                #endif
//                return false;
//                
//            case FR_NOT_OPENED:
//                #if defined (debug)
//                    Serial.println(F("sdc_setup ERROR pf_readdir failed with FR_NOT_OPENED"));
//                #endif
//                return false;
//
//        } // case
//        
//        if ( fileinfo.fname[0] == 0) break; // we already read all items in dir
//
//        // check that the items (= subdirs of root) are the right ones...
//        if ( strlen(fileinfo.fname) == 1 ) {
//            if ( ( fileinfo.fname[0] >= KEYSCAN_1 ) && ( fileinfo.fname[0] <= KEYSCAN_9 ) ){
//                #if defined (debug)
//                    Serial.print(F("sdc_setup INFO pf_readdir found dir "));
//                    Serial.println(fileinfo.fname);
//                #endif
//            } else {
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_readdir found a not allowed rootentry "));
//                    Serial.println(fileinfo.fname);
//                #endif
//                return false;
//            }
//        } else {
//            if ( strcmp(fileinfo.fname, SDC_SYSTEM_MESSAGES_DIR) == 0 ) {
//               #if defined (debug)
//                    Serial.print(F("sdc_setup INFO pf_readdir found dir "));
//                    Serial.println(fileinfo.fname);
//                #endif
//            } else {
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR 2 pf_readdir found a not allowed rootentry "));
//                    Serial.println(fileinfo.fname);
//                #endif
//                return false;
//            }
//        }
//
//        i++;
//    }
//
//    if ( i != 10 ) {
//        #if defined (debug)
//            Serial.print(F("sdc_setup ERROR pf_readdir found more than the allowed 10 dirs, it found "));
//            Serial.println(i);
//        #endif
//        return false;
//    }
//  	
//  	 
//  
//    // for each dir, get track-number-min and -max, see 010_globals for explanations.
//    // if no file in dir --> return "255" for both
//    // otherwise return number of "lowest" and the "highest" file in dir
//    i = 0;
//    for (c=KEYSCAN_1 ; c <= KEYSCAN_9 ; c++) {
//
//        dirpath[0] = c;
//        dirpath[1] = '\0';
//        
//        // goto relevant dir
//        result = pf_opendir( &directory, dirpath );
//        switch ( result ){
//        
//            case FR_OK:
//                // do nothing...
//                break;
//                
//            case FR_NO_FILE:
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_opendir "));
//                    Serial.print(dirpath);
//                    Serial.println(F("failed with FR_NO_FILE"));
//                #endif
//                return false;
//                
//            case FR_NOT_READY:
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_opendir "));
//                    Serial.print(dirpath);
//                    Serial.println(F("failed with FR_NOT_READY"));
//                #endif
//                return false;
//                
//            case FR_DISK_ERR:
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_opendir "));
//                    Serial.print(dirpath);
//                    Serial.println(F("failed with FR_DISK_ERR"));
//                #endif
//                return false;
//                
//            case FR_NOT_ENABLED:
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_opendir "));
//                    Serial.print(dirpath);
//                    Serial.println(F("failed with FR_NOT_ENABLED"));
//                #endif
//                return false;
//        } // switch         
//
//        // reset variables
//        player_track_number_min[i] = 255;
//        player_track_number_max[i] = 255; 
//
//        // i is the number of the dir we are in
//        // j is used as another counter for the filenumbers in this dir
//
//        // so now we scan through this dir and get files with filenames that are to be of format "xxx.WAV"
//        // max. 254 such files are allowed
//        // no other files are allowed
//        // we do not know the sequence in which files are scanned
//        j = 0;
//        for (;;){
//            result = pf_readdir(&directory, &fileinfo);
//            switch ( result ){
//        
//                case FR_OK:
//                    // do nothing...
//                    break;
//                    
//                case FR_DISK_ERR:
//                    #if defined (debug)
//                        Serial.println(F("sdc_setup ERROR pf_readdir failed with FR_DISK_ERR"));
//                    #endif
//                    return false;
//                    
//                case FR_NOT_OPENED:
//                    #if defined (debug)
//                        Serial.println(F("sdc_setup ERROR pf_readdir failed with FR_NOT_OPENED"));
//                    #endif
//                    return false;
//    
//            } // case
//            
//            if ( fileinfo.fname[0] == 0) break; // we already read all items in dir
//
//            // if found thing is a dir --> error
//            if ( fileinfo.fattrib & AM_DIR ){
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_readdir found a dir "));
//                    Serial.print(dirpath);
//                    Serial.print(F("/"));
//                    Serial.println(fileinfo.fname);
//                #endif
//                return false; 
//            }
//    
//            // check that the items (= files in dir) are the right ones...
//            filenamelength = strlen(fileinfo.fname); // strlen == length without terminating '\0'
//            if ( ( filenamelength < 5 ) || ( filenamelength > 7 ) ) {
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_readdir found a file with too short or long filename "));
//                    Serial.print(dirpath);
//                    Serial.print(F("/"));
//                    Serial.println(fileinfo.fname);
//                #endif
//                return false;
//            }
//            
//            // ok right length
//            // check that last 4 chars are ".WAV"
//            if ( (fileinfo.fname[filenamelength - 1] != 'V') ||
//                 (fileinfo.fname[filenamelength - 2] != 'A') ||
//                 (fileinfo.fname[filenamelength - 3] != 'W') ||
//                 (fileinfo.fname[filenamelength - 4] != '.') ) {
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_readdir found a file with wrong suffix "));
//                    Serial.print(dirpath);
//                    Serial.print(F("/"));
//                    Serial.println(fileinfo.fname);
//                #endif
//                return false;
//            }
//                        
//            // ok, cut off last four chars
//            fileinfo.fname[filenamelength - 4] = '\0';
//            filenamelength = strlen(fileinfo.fname); // strlen == length without terminating '\0'
//            if (  ( ! isDigit(fileinfo.fname[0]) ) ||
//                  ( filenamelength > 1 && ! isDigit(fileinfo.fname[1]) ) ||
//                  ( filenamelength > 2 && ! isDigit(fileinfo.fname[2]) ) ) {
//                #if defined (debug)
//                    Serial.print(F("sdc_setup ERROR pf_readdir found a file with non-number filename "));
//                    Serial.print(dirpath);
//                    Serial.print(F("/"));
//                    Serial.print(fileinfo.fname);
//                    Serial.println(F(".WAV"));
//                #endif
//                return false;
//            }
//            fileNumber = atoi( fileinfo.fname ); // fileNumber is 16bit unsigned int in order to find e.g. 367.WAV as errornous file...
//            if ( fileNumber > 254 ) {
//                #if defined (debug)
//                    Serial.println(F("sdc_setup ERROR pf_readdir found a file with filename > 254.WAV "));
//                #endif
//                return false;
//            }
//
//            // only for first file, when player_track_number_max is on initial 255 value
//            if ( player_track_number_max[i] == 255 ) {
//                player_track_number_max[i] = fileNumber;
//            } 
//
//            if ( fileNumber > player_track_number_max[i] ) {
//                player_track_number_max[i] = fileNumber;
//            } 
//            
//            if ( fileNumber < player_track_number_min[i] ) {
//                player_track_number_min[i] = fileNumber;
//            } 
//
//            // and a very last check: we don't want to have more than 255 files max! (0 .. 254)
//            // thus if j == 255 we have a problem...
//            if ( j == 255 ){
//                #if defined (debug)
//                    Serial.println(F("sdc_setup ERROR pf_readdir found more than 255 files in one dir "));
//                #endif
//                return false;
//            } 
//
//            j++;
//        } // next file in this dir please!
//    
//        #if defined (debug)
//            Serial.print(F("sdc_setup INFO player_track_number_min and max for playlist "));
//            Serial.print(c);
//            Serial.print(F(" are "));
//            Serial.print(player_track_number_min[i]);
//            Serial.print(F(" and "));
//            Serial.println(player_track_number_max[i]);
//        #endif
//        
//        i++; 
//    } // next dir please!
//
//    #if defined (debug)
//        Serial.println(F("sdc_setup INFO finished."));
//    #endif
//
//    return true;
//  
//} // sdc_setup()
