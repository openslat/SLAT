#!/usr/bin/env python3
import sys
import io
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser

# http://www.antlr.org/wiki/display/ANTLR3/Python+runtime
# http://www.antlr.org/download/antlr-3.1.3.jar

def main(argv):
    test_cases = [ 'title \'This is a title\';',
                   'title \'This is a title with a \\\' quote inside.\';',
                   'title \'This is a title with a \\\\ backslash.\';',
                   'title \'This is a title with a \\\' quote and a \\\\ backslash.\';',
                   'detfn detfn1 hyperbolic 1.0, 2, -.5;',
                   'detfn detfn1 powerlaw 0.5, 1.2;'
    ]
    for test_case in test_cases:
        print("'",test_case, "' --> ", sep='', end='')
        input = InputStream(test_case)
        lexer = slatLexer(input)
        stream = CommonTokenStream(lexer)
        parser = slatParser(stream)
        tree = parser.script()
        print(tree.toStringTree(recog=parser))

if __name__ == '__main__':
    main(sys.argv)
