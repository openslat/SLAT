Seismic Loss Assessment Tool

SLAT is a computer program for performing seismic loss assessment of structures
subjected to earthquake-induced hazards. This code is based on the original
FORTAN version (available from [here](https://sites.google.com/site/brendonabradley/software/seismic-performance-and-loss-assessment-tool-slat)). The tool uses the uncoupled modelling
approach advocated by the Pacific Earthquake Engineering Research (PEER) Centre,
where the loss is computed through the use of interim variables.

This project is in very early stages; at present it is little more than a
collection of classes for representing a few of the relationships needed for
loss assessment.

# Requirements

Current requirements include:

-   the GNU g++ compiler
-   GNU make
-   libboost libraries
-   the GNU Scientific Library

# Demo

The script 'demo.sh' will build and run the 'main' executable, which generates
data for sample IM-Rate and EDP-IM relationships, as well as deriving data for
the resulting EDP-Rate relationship. The script then plots the data using
GNU's *plotutils*, and displays the plots (using `qiv`).

# Tests

The 'unit<sub>tests</sub>' tarket builds the unit tests as `unit_tests`. This executable
runs some basic unit tests on the defined classes and integration functions.
