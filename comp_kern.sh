#!/bin/bash
make
echo Removing kernel mod
rmmod procAncestry_kernel.ko
echo Inserting kernel mod
insmod procAncestry_kernel.ko