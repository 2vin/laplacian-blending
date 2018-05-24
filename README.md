# laplacian-blending
Using laplacian blending to blend two image seamlessly     

This program can be used to blend two images seamlessly using OpenCV. 

# Compile
g++ laplacian-blending.cpp -o main `pkg-config --cflags --libs opencv` -std=c++11

# Usage 
./main

# Results
 Input images #1 Apple    
 ![alt text](https://raw.githubusercontent.com/2vin/laplacian-blending/master/results/apple.jpg)   
 
  Input images #2 Orange    
 ![alt text](https://raw.githubusercontent.com/2vin/laplacian-blending/master/results/orange.jpg)   
 
  Output Image    
 ![alt text](https://raw.githubusercontent.com/2vin/laplacian-blending/master/results/blended.jpg)   
 
