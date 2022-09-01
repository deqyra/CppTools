#!/usr/bin/bash

shopt -s extglob
rm -rf ./*
rm -rf ./../external/include/!(catch2)
shopt -u extglob
