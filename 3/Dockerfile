FROM gcc:11
RUN apt-get update && apt-get install -y cmake libopencv-dev rapidjson-dev libmosquittopp-dev

COPY . /usr/src/3
WORKDIR /usr/src/3

RUN mkdir build && cd build && cmake .. && make
CMD ./bin/3
