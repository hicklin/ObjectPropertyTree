# Object and pointer property tree

This repository contains the source code for the object property tree library. With this library it is easy to create a 
property tree where objects and pointers can be stored. Nodes can be set and accessed via dot delimited strings.

This library can be particularly useful in a multi-threaded application where objects need to be accessed by different 
threads.


## Building

This library is build using cmake. The library depends on Boost so make sure that you have it installed. To build:

1.  Change directory to the repository's root directory.
2.  Make a build directory `mkdir build`.
3.  `cd build`.
4.  Run cmake `cmake ..`.
5.  Make `make -j 4`.
6.  Install `sudo make install`.


## Documentation

Documentation can be generated using doxygen

1.  Install doxygen `sudo apt install doxygen`.
2.  Change directory to the repository's root directory.
3.  Run doxygen `doxygen Doxyfile`.
4.  Open the `docs/html/index.html` file with you browser.
5.  Read the docs.


## Tests

A tests application is supplied with the library. To build it set the variable `BUILD_TESTS` ON. 
```cmake
cmake -DBUILD_TESTS=ON
```
This test application can be executed to ensure that the library is working as intended on your system.

The source code of the tests are located in `src/tests`. The tests for the ObjectPropertyTree class can be used as an 
**example** of the use of the library.


## Further development

The underlying PropertyTree class is templated such that it will allow any key and value types. This can be used as is
or used to facilitate the creation of new types of property trees. If you think that a property tree design you 
developed based on this library can be useful to others, get in touch.


## Contributions

This library is based from work that was developed by Barry Hill.
