# Thumbnail Preserving Encryption
A WebAssembly version of the Enhanced Thumbnail Encryption project found at https://github.com/osucybersec/tpe-demo. 
Scrambles pixels in a provided image while preserving the thumbnail of the image. 

### Dependencies 
- Emscripten --> download and install into the same directory as the repository, as emsdk. Follow instructions for setup
- Node.js
- express

### To run
./build.sh

## Pre-Processing
This repo contains a script that is not directly called in the TPE site, but can be used to increase security on an image. The script takes an image, as well as a target (usually this is the image with a filter applied) and modifies the given image such that the thumbnail of the original images matches that of the filtered image. 

### To run pre-processing
In the code: change the source, destination, and filter folders to those where your images reside
In console: python3 pre-processing.py
