### 2.2
```
docker build . -t grillow1337/2.2
xhost +
docker run -it --rm -e DISPLAY -v /tmp/.X11-unix/:/tmp/.X11-unix --net=host --device /dev/snd grillow1337/2.2
xhost -
```
![Result](result.png)
