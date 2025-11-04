#!/bin/bash



for fileGBA in *.gba
do
    echo "----------------------------------"
    fileXML=`mktemp XXXXX.xml`
    md5=`md5sum "$fileGBA"  | awk '{ print $1 }'`
    crc=`crc32 "$fileGBA"`
    # echo $md5" - "$fileGBA
    echo $fileGBA
    wget -q -O $fileXML "https://api.screenscraper.fr/api2/jeuInfos.php?devid=$SSDEVID&devpassword=$SSDEVPASSWD&output=xml&ssid=$SSID&sspassword=$$SSPASSWORD&md5=$md5&crc=$crc"
    echo $crc" - "$md5" - "`grep romid $fileXML`
    #rm -f $fileXML
done
