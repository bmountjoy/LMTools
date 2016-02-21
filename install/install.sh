#!/bin/sh
echo "Installing LMTools"
echo "sudo mkdir -p /usr/local/lib"
sudo mkdir -p /usr/local/lib
echo "sudo cp -n *.dylib /usr/local/lib"
sudo cp -n *.dylib /usr/local/lib
echo "sudo mkdir -p ~/Desktop/LMTools"
sudo mkdir -p ~/Desktop/LMTools
echo "cp LMTools.zip ~/Desktop/LMTools"
cp LMTools.zip ~/Desktop/LMTools
echo "unzip ~/Desktop/LMTools/LMTools.zip -d ~/Desktop/LMTools/"
unzip ~/Desktop/LMTools/LMTools.zip -d ~/Desktop/LMTools/
echo "Done!"