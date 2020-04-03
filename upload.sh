#!/bin/bash -e

pushd $(dirname $0) &>/dev/null
DIR=$(pwd)
popd &>/dev/null

HOST="192.168.0.66"
BASEURL="http://${HOST}"
SL=0.1

FW="${DIR}/cmake-build-debug/fancontrol.bin"
FW_FILENAME=$(basename $FW)
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
	curl -s -F "data=@${FW}" $BASEURL/upload | grep "Uploaded OK" >/dev/null
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

espOTAupdate() {
	file=$1
	fn=$(basename $file)
	curl -s -F "firmware=@${file};filename=\"${fn}\"" ${BASEURL}/update
}

mcuReboot
mcuSyncBootloader
mcuDeleteBin
mcuUploadBin
mcuErase
mcuFlash
mcuReboot
