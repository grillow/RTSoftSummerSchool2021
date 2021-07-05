### 1.1
<pre>
1. Inject the flashdrive
2. Enjoy <flashdrive>/<timestamp>/log
</pre>
### 1.2
```
make
sudo insmod ADC_Test.ko
sudo mknod /dev/ADC_Test c $(cat /proc/devices | grep ADC_Test | awk '{print $1}') 0
sudo dd if=/dev/ADC_Test of=/dev/stdout count=1 2>/dev/null
sudo rm /dev/ADC_Test
sudo rmmod ADC_Test
```
