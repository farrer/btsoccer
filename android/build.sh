#!/bin/bash

rm -r ./assets/data
cp -R ../data ./assets
ndk-build
ant debug install

