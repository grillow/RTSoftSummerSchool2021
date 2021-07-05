#!/usr/bin/bash
mkdir -p /mnt/${ID_FS_UUID}
sudo /usr/bin/mount UUID=${ID_FS_UUID} /mnt/${ID_FS_UUID}

DESTINATION=/mnt/${ID_FS_UUID}/$(date +"%s")
echo ${DESTINATION}
mkdir ${DESTINATION}

cp -r /var/log/ ${DESTINATION}

sync /mnt/${ID_FS_UUID}
sudo /usr/bin/umount /mnt/${ID_FS_UUID}
rm -r /mnt/${ID_FS_UUID}
