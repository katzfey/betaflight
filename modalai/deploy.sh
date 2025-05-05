#!/bin/bash

adb push test.so /usr/lib/rfsa/adsp/betaflight.so
adb push src/platform/HEXAGON/host/betaflight /usr/bin
adb shell chmod a+x /usr/bin/betaflight
