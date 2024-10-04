#!/bin/bash

# Compile the C++ program
g++ -std=c++17 image_checker.cpp -o image_checker

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."
    # Run the program
    ./image_checker
else
    echo "Compilation failed."
fi
