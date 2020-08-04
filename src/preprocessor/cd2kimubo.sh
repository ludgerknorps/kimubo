#!/bin/bash



defDEBUG=0 		# 1 --> show debug level in console
				# 0 --> no show debug level
				
debugFile="debug.txt"	
			
if [[ "$defDEBUG" == "1" ]]
then
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++ " >> "$debugFile"
	echo "++++++++++++++  " $(date) >> "$debugFile"
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++ " >> "$debugFile"
fi
					
function print_debug {
  [ $defDEBUG -eq 1 ] && echo "$1" | tee -a "$debugFile"
}

#########################
# Greeting 				#
#########################

greeting(){
    echo "KIMUBO"
    echo "KInderMUsikBOx"
    echo " "
    echo "	Ein minimalistischer WAV/PCM Spieler für Kinder - und mehr."
    echo "	(c) 2019 ludgerknorps <56650955+ludgerknorps@users.noreply.github.com>"
    echo " "
    echo "This Bash script creates \"Kimubo-ready\" files either by ripping from a CD or by converting existing WAV/OGG files into Kimubo format."
    echo " "	
}

#########################
# The command line help #
#########################
display_help() {

    echo "Options:"
	echo "	-w <path_of_working_dir>	where ogg and wav files are stored to"
	echo "	-d <path_of_cdrom_device>	which cdro to use for ripping"
	echo "	-m 				if set: multidisk, i.e. more than one disk are to be ripped into one workingdir"
	echo "	-n 				if set: no rip, just convert existing files"
	echo "						these files must be RIP1.wav, RIP2.wav, ... or RIP1.ogg, RIP2.ogg, ..."
	echo "	-o <number>			offset for tracks from this cd, needs to be provided if \"-m\" option is given; must only be given together with \"-m\" option"
	echo "	-a				\"automatic\" mode: if no NNN.WAV files exist in working dir, then assume it is first disk and thus behave like without -m option (implying -o 0)"
	echo "						but if NNN.WAV files exist in working dir, then assume it is a follow up disk and thus behave like with -m -o NNN"
	echo "  -c              no CDDB lookup for rip"
	echo "  NOT YET -r <number>     renumber existing files; this is used, if you rip several disks and each rip results in 1.wav .. n.wav; from the second disk on you"
	echo "                      use -r n+1 and all new files (1.wav .. n.wav) will be renamed to RIPn+1.wav .."
	echo "                      inplies -n"
	echo "  NOT YET -b              add \"RIP\" to all existing 1.wav .. n.wav (actually this uses \"-r 0\" internally and thus inplies -n)"
	echo "						if -b and -r N are given together, then -r N has higher priority"
	echo "	"
	echo "	Examples:"
	echo "		cd2kimubo.sh -w /home/ludger/music/toKimubo -d /dev/sr1 -m -o 12 "
	echo "		Rip disk in /dev/sr1, do cddb lookup for ARTIST and ALBUM, put files into /home/ludger/music/toKimubo/ARTIST_ALBUM/ and start naming files there with 12.ogg"
	echo "	"
}


# Variables (and is applicable their defaults)

vWorkingDir="null"
vCDDevice="null"
vIsMultiDisk="0"
vTrackOffsetNr="null"
vNoRip="0"
vIsAutoMode="0"
vIsRenumber="null"



# print greeting
greeting

# check parameters and options given in command line, only continue if successful and sensible
while getopts d:w:mno:ar:bc opt
do
   case $opt in
		w) 	vWorkingDir="$OPTARG"
			;;
		d) 	vCDDevice="$OPTARG"
			;;
		m) 	vIsMultiDisk="yes"
			;;
		n) 	vNoRip="yes"
			;;
		o)	vTrackOffsetNr="$OPTARG"
			;;
		a)	vIsAutoMode="yes"
			;;
		b)	vIsRenumber="0"
			vNoRip="yes"
			# uses "-r 0" internally, thus b option goes to vIsRenumber
			;;
		r)	vIsRenumber="$OPTARG"
			vNoRip="yes"
			;;
		c)  vCDDBOption="--nocddb"
			;;
		
	
   esac
done

# sanity checks
	if [[ "$vWorkingDir" == null ]]
	then
		echo -e $vHelpText
		display_help
		echo "ERROR: -w option and parameter are mandatory (e.g.   -w /this_dir   )   ... exiting..."
		exit 1
	fi

	if [[ "$vCDDevice" == null ]]
	then
		echo -e $vHelpText
		echo "INFO: -d option not given, therefore using /dev/cdrom"
		vCDDevice="/dev/cdrom"
	fi

	if [[ "$vIsMultiDisk" == yes ]] && [[ "$vTrackOffsetNr" == null ]]
	then
		echo -e $vHelpText
		echo "ERROR: -m option requires -o option and parameter (e.g.   -m -o 12   )   ... exiting..."
		exit 1
	fi
	
	if [[ "$vIsMultiDisk" == 0 ]] && [[ "$vTrackOffsetNr" != null ]]
	then
		echo -e $vHelpText
		echo "ERROR: -o option must only be given together with -m option   ... exiting..."
		exit 1
	fi
	
	if [[ "$vIsAutoMode" == yes ]] && [[ "$vIsMultiDisk" == yes ]]
	then
		echo -e $vHelpText
		echo "ERROR: -a option forbids -m and -o options   ... exiting..."
		exit 1
	fi
	
	if [[ "$vIsRenumber" != null ]] && [[ "$vIsAutoMode" == yes ]]
	then
		echo -e $vHelpText
		echo "ERROR: -r and -b options forbids -a option   ... exiting..."
		exit 1
	fi

# cannonicalize working dir path (resolve relative paths)
	vWorkingDirAbsolute=$( readlink -f "$vWorkingDir" )
	
	# returns 0 on error (all path-parts except last need to exist!)
	if [[ -z "$vWorkingDirAbsolute" ]]
	then
		echo -e $vHelpText
		echo "ERROR: working dir (provided with -w option) $vWorkingDirAbsolute is not correct (all path-parts except last need to exist!)   ... exiting..."
		exit 1
	fi
	
# create last part of working dir, if needed
	# because we used readlink command we don't have to worry about symlinks...
	if [[ ! -d "$vWorkingDirAbsolute" ]]
	then
		if [[ -f "$vWorkingDirAbsolute" ]]
		then
			echo "ERROR: working dir (provided with -w option) $vWorkingDirAbsolute is a file ?!  ... exiting..."
			exit 1
		else
			mkdir "$vWorkingDirAbsolute"
			if [[ -d "$vWorkingDirAbsolute" ]]
			then
				echo "INFO: created working dir $vWorkingDirAbsolute"
			else
				echo "ERROR: tried to create working dir (provided with -w option) $vWorkingDirAbsolute --- but I failed (maybe permissions?)  ... exiting..."
			exit 1
			fi
		fi
	fi
		
# change into working dir
	if [[ "$PWD" == "$vWorkingDirAbsolute" ]]
	then
		echo "INFO: working dir is current dir $vWorkingDirAbsolute"
	else
		echo "INFO: changing into working dir $vWorkingDirAbsolute"
		cd "$vWorkingDirAbsolute"
	fi

# rip cd to high quality wav filesnames are RIP1.wav .. RIPNNN.wav
	if [[ "$vNoRip" != yes ]] 
	then
		pacpl --rip all --nocddb --to wav --overwrite --nscheme="RIP%tr" -v --noinput --outdir "$vWorkingDirAbsolute" --device "$vCDDevice" 
	else
		echo "INFO: skipping rip (as -n option is provided!)"
	fi 

# reinitialize the vTrackOffsetNr 
	if [[ "$vTrackOffsetNr" == null ]] 
	then
		vTrackOffsetNr=0
	fi	
	
# if automatic mode, do the magic...
	if [[ "$vIsAutoMode" == yes ]] 
	then
		echo "INFO: automatic mode enabled..."
		
		# list all WAV files with "natural" ordering (-v, i.e. 1.WAV, 9.WAV, 11.WAV, ...) and ignore if there are no WAV files (2>>/dev/null)
		# then take only last WAV file (== one with highest natural number)
		vMaxNumberWavFile=$( ls *.WAV -1v 2>>/dev/null | tail -1 )
		if [[ -z "$vMaxNumberWavFile" ]]
		then 
			# no WAV files or only one with 0.WAV (which does not matter...)
			vTrackOffsetNr=0
		else
			# $vMaxNumberWavFile now holds something like NNN.WAV, we only need the NNN part, so...
			vTrackOffsetNr="${vMaxNumberWavFile::-4}"
		fi
		echo "INFO: ... using file-number-offset of $vTrackOffsetNr"
	fi	
	
## if rename mode
	#if [[ "$vIsRenumber" != null ]] 
	#then
		#echo "INFO: renumber mode enabled..."
		
		#vTrackOffsetNr="$vIsRenumber"

		#echo "INFO: ... using file-number-offset of $vTrackOffsetNr"
	#fi	
	
	#for theRippedFile in RIP*.wav
	
# make oggs from high quality wavs
	for theRippedFile in *.wav
	do
		if [[ "${theRippedFile::3}" == "RIP" ]]
		then
			# reconvert to ogg quality level 6 (~=192kbps)
			echo "INFO: ... converting to ogg file ${theRippedFile::-4}.ogg"
			sox "$theRippedFile" -C6 "${theRippedFile::-4}.ogg"
		fi
	done
	
# mv ogged files to correct offset-filenames 
# and convert to KIMUBO-correct (lower quality) WAV

	for theOggFile in *.ogg
	do
		if [[ "${theOggFile::3}" == "RIP" ]]
		then
			oldNumber=${theOggFile:3:-4}
			newNumber=$((oldNumber + vTrackOffsetNr))
			mv -v "$theOggFile" "$newNumber.ogg"
			
			# reconvert to mono WAV 16bit 32kSPS
			sox "$newNumber.ogg" -r 32000 -b 16 -c 1 "$newNumber.WAV" remix - gain -n -2
		fi
	done
	
# delete temporary files

	rm RIP*.wav



exit 0

 
