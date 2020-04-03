#!/bin/bash -e

pushd $(dirname $0) &>/dev/null
DIR=$(pwd)
popd &>/dev/null

HOST="192.168.0.66"
BASEURL="http://${HOST}"
SL=0.1

SEP="\t\t\t\t"

mcuSyncBootloader() {
	echo -ne "Syncing bootloader $SEP "
	curl -s $BASEURL | grep "STM32F103" >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"
	return $ret
}

mcuReboot() {
	echo -ne "Rebooting $SEP "
	curl -s $BASEURL/run | grep "Run" >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"
	return $ret
}

mcuUploadBin() {
	echo -ne "Uploading binary $SEP "
	fw=$1
	curl -s -F "data=@${fw}" $BASEURL/upload | grep "Uploaded OK" >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"
	return $ret
}

mcuErase() {
	echo -ne "Erasing flash $SEP "
	curl -s $BASEURL/erase | grep "Erase OK" >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"
	return $ret
}

mcuFlash() {
	echo -ne "Flash binary $SEP "
	curl -s $BASEURL/programm | grep "Programming" >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"

	return $ret
}

mcuDeleteBin() {
	echo -ne "Deleting binary $SEP "
	curl -s $BASEURL/delete >/dev/null
	ret=$?
	[ $ret -eq 0 ] && echo "Done" || echo "FAIL"
	return $ret
}

mcuBinStatus() {
	TMP=$(tempfile)
	curl -s ${BASEURL}/list 2>/dev/null >$TMP

	export MCU=$(cat $TMP | sed 's|.*MCU: \([a-zA-Z0-9]*\).*|\1|g')
	export FILE=$(cat $TMP | sed 's|.*File: \{1,2\}\([a-zA-Z0-9\.]*\).*|\1|g')
	export SIZE=$(cat $TMP | sed 's|.*Size:\(0-9\)+.*|\1|g')

	rm $TMP	
}

printCSum() {
	file=$1
}

espOTAupdate() { 
	fw=$1
	[ ! -f $fw ] && echo >&2 "Firmware '$fw' not found" && return 1

	fn=$(basename $fw)
	lu=${fw}.lastUpdate
	cs=$(sha1sum -b ${fw} | cut -f1 -d' ')
	
	[[ -f "$lu" ]] && [[ "$cs" == "$(cat $lu)" ]] && echo "FW already up to date" && return 0

	echo -ne "Updating ESP $SEP "
	curl -s -F "firmware=@${fw};fwname=\"${fn}\"" ${BASEURL}/update | grep "Update Success" >/dev/null
	ret=$?
	if [ $ret -eq 0 ]; then
	       	echo "Done"
		sha1sum -b $fw | cut -f1 -d' ' > $lu
	else
		echo "FAIL"
	fi
}

stm32OTAupdate() {
	fw=$1

	[ ! -f $fw ] && echo >&2 "Firmware '$fw' not found" && return 1

	fn=$(basename $fw)
	lu=${fw}.lastUpdate
	cs=$(sha1sum -b ${fw} | cut -f1 -d' ')
	
	[[ -f "$lu" ]] && [[ "$cs" == "$(cat $lu)" ]] && echo "FW already up to date" && return 0

	mcuReboot
	mcuSyncBootloader
	mcuDeleteBin
	mcuUploadBin $fw
	mcuErase
	mcuFlash
	mcuReboot

	sha1sum -b $fw | cut -f1 -d' ' > $lu
}

if [ $1 == "--stm32" ]; then
	stm32OTAupdate $DIR/cmake-build-debug/fancontrol.bin
fi

if [ $1 == "--esp" ]; then
	espOTAupdate /tmp/arduino_build_67208/STM32-OTA-ESP8266.ino.bin
fi

