# AlFreD : Alignment Free Distance Estimator

Compute ACS using the prefix-chopping algorithm for every pair of strings present in a database of strings.

## Dependencies
### External Dependencies that need to be installed
* A modern, C++11 ready compiler such as `g++` version 4.8 or higher or `clang` version 3.2 or higher.
* The [cmake](www.cmake.org) build system (Version >= 2.8.11).
* A 64-bit operating system. Either Mac OS X or Linux are currently supported.
* Git version control system

### Dependices present as submodule.
* [googletest](https://code.google.com/p/googletest/) is included as git submodule.
* [libdivsufsort](https://github.com/y-256/libdivsufsort) libraries are copied in the directory.

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

Place the input sequences into fasta files, with each sequence placed in a separate file, in a directory. Run the program while providing this input directory as -i option, the output file as the -o option, and the number of mismatches to allow is given as -k option.
An example is shown below:

    build/alfred.x -i data/primates/ -o acs.primates.k3.out -k 3

The above command runs the ACS_k prefix-chopping algorithm for the input
files in the directory "data/primates" and produces the output to
the file "acs.primates.k3.out". It allows three mismatches, while
matching for longest common prefix in the ACS_k computation.

If you want to run with a specific set of input files, then you can run as follows:

    build/alfred.x -f data/filex.fa,data/filey.fa -o acs.primates.k3.out -k 3

## License

Apache License 2.0
