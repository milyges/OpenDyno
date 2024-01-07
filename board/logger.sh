#!/bin/sh

set -e

stty -F /dev/ttyUSB0 115200 -isig -icanon -iexten -echo -opost
cat /dev/ttyUSB0 | tee "log.$(date '+%Y%m%d-%H%M%S').txt"
