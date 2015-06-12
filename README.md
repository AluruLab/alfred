# Arakawa

Compute ACS using the prefix-chopping algorithm for every pair of strings present in a database of strings.

## Dependencies
### External Dependencies that need to be installed
* A modern, C++11 ready compiler such as `g++` version 4.8 or higher or `clang` version 3.2 or higher.
* The [cmake](www.cmake.org) build system (Version >= 2.8.11).
* A 64-bit operating system. Either Mac OS X or Linux are currently supported.
* Git version control system

### Dependices present as submodule.
* [googletest](https://code.google.com/p/googletest/) is included as git submodule.
* [libdivsufsort](http://github.com/stxxl/stxxl) libraries are copied in the directory.

## Compilation

Initialize submodules as below, if they are not already initialized.

    git submodule init
    git submodule update

Next, Create a build directory. For example,

     mkdir build
     cd build

Finally, build the executable 'arakawa.x'.

     cmake ..
     make

## Running

Place the files in a directory and run with the -i option for input directory. -o option is for output file and -k is provided to allow the number of errors.
An example shown below

    build/arakawa.x -i data/primates/ -o acs.primates.k3.out -k 3

The above command runs the ACS_k prefix-chopping algorithm for the input
files given in the directory "data/primates" and produces the output to
the file "acs.primates.k3.out". It allows three mismatches, while
matching for longest common prefix in the ACS_k computation.
