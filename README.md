### 1.1
```
1. Inject the flashdrive
2. Enjoy <flashdrive>/<timestamp>/log
```
### 1.2
```
make
sudo insmod ADC_Test.ko
sudo mknod /dev/ADC_Test c $(cat /proc/devices | grep ADC_Test | awk '{print $1}') 0
sudo dd if=/dev/ADC_Test of=/dev/stdout count=1 2>/dev/null
sudo rm /dev/ADC_Test
sudo rmmod ADC_Test
```
### 2.1
```
docker build . -t grillow1337/x11-forwarding-example
xhost +
docker run -it --rm -e DISPLAY -v /tmp/.X11-unix/:/tmp/.X11-unix --net=host --device /dev/snd grillow1337/x11-forwarding-example
xhost -
```
