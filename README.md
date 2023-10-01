# function-boundaries-predictor
A program using prefix-tree model to predict basic block boundaries of x86 binary.

## Introduction
The process of translating high-level languages into low-level machine codes through a compiler can result in the loss of some high-level information, such as function structures. This can make it challenging when analyzing binaries. In this study, we employ a Weight Prefix Tree to analyze X86 binaries with the aim of predicting the starting and ending positions of High-Level Language (HLL) functions within the binaries to assist in optimizing binaries.

## Environment
- OS: 
    - Ubuntu 14.04 32-bit
- Compiler
    - gcc 4.8.4 tar=i686-linux-gnu
## Dataset - SPEC CPU 2006
Compiled C source codes into training data using GCC optimization levels o0 to o2. The correct function boundaries are extracted from header files as the target for training. 
### Method
Two Weight Prefix Tree structures are constructed
- one for predicting the beginning of functions
- the other one was built in reverse order for predicting the end of functions

## Compile SPEC program O0~O2.
1. Move ```megaca_o0.cfg```, ```megaca_o1.cfg```, ```megaca_o2.cfg```, and ```buildSPEC.sh``` to ```/SPEC_CPU2006/config```.
2. Modify the path inside ```buildSPEC.sh``` as follows:
    ```sh
    DIR="/SPEC_CPU2006/benchspec/CPU2006/"
    DESDIR="/workspace/"
    ```
3. Run the script
    ```bash
    cd /SPEC_CPU2006/config
    bash buildSPEC.sh
    ```

## Execution
```bash
g++ pfx_tree.cpp -o pfx_tree
./pfx_tree
```
