This directory containts the Windows 64-bit release of OpenSLAT.

OpenSLAT requires Python3, and the antlr4 python support. I've been using the
Anaconda version of python, since it includes all the numeric and scientific
libraries I ever expect to need. Anaconda is available from
https://www.continuum.io/downloads.

Once python is installed, you can install antlr4 support by running:

    > pip install antlr4-python3-runtime

from a command shell.

The 'bin' directory contains all the library files needed to run OpenSLAT.

The 'example2' directory contains the data files for example #2 from the
original SLAT project, and includes three ways to perform the assessment:
	 - example2.EXE is a C++ program that uses the OpenSLAT
	   library directly
	 - example2.py is a Python program, use the Python 
	   interface to the OpenSLAT library
	 - example2.slat is written as an OpenSLAT script, which
	   can be interpreted by the SlatInterpreter.py Python
	   script in the bin directory.

All three examples can be run by opening a command window, making this the
active directory, and executing 'runme.bat'. This produces three subdirectories
in the example2 folder:
          - 'results' contains the output of the OpenSLAT script
	  - 'py-results' contains the output fo the Python script
	  - 'c-results' contains the output of the C++ program
