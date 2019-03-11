Seismic Loss Assessment Tool

SLAT is a computer program for performing seismic loss assessment of structures
subjected to earthquake-induced hazards. This code is based on the original
FORTAN version (available from [here](https://sites.google.com/site/brendonabradley/software/seismic-performance-and-loss-assessment-tool-slat)). The tool uses the uncoupled modelling
approach advocated by the Pacific Earthquake Engineering Research (PEER) Centre,
where the loss is computed through the use of interim variables.

This project provides:
     - libslat, a shared library for the calculation engine
     - pyslat.py, Python bindings to libslat
     - SlatInterpreter.py, a Python-based interpreter for a
       domain-specific language for defining and evaluating
       seismic loss assessments.

The interpreter has been neglected, and has never been thoroughly tested or used. It may
still be of interest to seismic researchers.

The Python bindings are used by [webslat](https://www.github.com/openslat/webslat). That project provides a web-based interface (using the django framework) to the functionality of libslat. That includes instructions for installing SLAT (and webslat) on a virtual machine; it is a good place to visit when setting up your build system for Linux.

This project includes makefiles for cross-compiling a Windows build on a Linux system, using the [MXE](https://mxe.cc/) toolset.

It should also be possible to build on a Windows system, using [mingw](http://mingw.org/), but I don't have access to a system to test this on at the moment.


