#!/bin/sh
echo "Installing LMTools"
echo "sudo mkdir -p /usr/local/lib"
sudo mkdir -p /usr/local/lib
echo "sudo cp -n *.dylib /usr/local/lib"
sudo cp -n *.dylib /usr/local/lib
echo "sudo mkdir -p ~/Desktop/LMTools"
sudo mkdir -p ~/Desktop/LMTools
echo "sudo cp LMTools.zip ~/Desktop/LMTools"
sudo cp LMTools.zip ~/Desktop/LMTools
echo "sudo unzip ~/Desktop/LMTools/LMTools.zip -d ~/Desktop/LMTools/"
sudo unzip ~/Desktop/LMTools/LMTools.zip -d ~/Desktop/LMTools/
echo "Done!"