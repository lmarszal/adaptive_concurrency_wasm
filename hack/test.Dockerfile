FROM ubuntu:bionic

COPY ./test_container.sh /

RUN ./test_container.sh
