FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -yq build-essential cmake libopencv-dev rapidjson-dev libmosquittopp-dev libboost-all-dev

COPY . /usr/src/5
WORKDIR /usr/src/5

RUN mkdir build && cd build && cmake .. && make
CMD ./bin/5
