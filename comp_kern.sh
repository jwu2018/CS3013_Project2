#!/bin/bash
make
echo Removing kernel mod
rmmod antivirus.ko
rmmod procAncestry_kernel.ko
echo Inserting kernel mod
insmod antivirus.ko
insmod procAncestry_kernel.ko