#!/bin/bash

echo "Compiling scc.c..."
gcc scc.c -o scc

if [ $? -eq 0 ]; then
    echo "Compilation successful. Installing to /usr/local/bin..."
    # Using /usr/local/bin as it's the standard path for user-installed binaries
    sudo cp scc /usr/local/bin/scc
    sudo chmod +x /usr/local/bin/scc
    
    echo "Cleaning up local build binary..."
    rm scc
    
    echo "Setting up Simple C Folder..."
    mkdir ~/simple_c
    cd ~/simple_c
    echo "Installation Complete! You can now run 'scc' directly from your terminal."
    echo "Make or Paste a .sc File into ~/simple_c to test it"
else
    echo "Error: Failed to compile scc.c"
    exit 1;
fi
