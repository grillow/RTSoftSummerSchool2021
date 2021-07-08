### 2.1
```
docker build . -t grillow1337/x11-forwarding-example
xhost +
docker run -it --rm -e DISPLAY -v /tmp/.X11-unix/:/tmp/.X11-unix --net=host --device /dev/snd grillow1337/x11-forwarding-example
xhost -
```
