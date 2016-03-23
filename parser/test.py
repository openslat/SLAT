#!/usr/bin/env python3
import sys
import io
from antlr4 import *
from dsltestLexer import dsltestLexer
from dsltestParser import dsltestParser

# http://www.antlr.org/wiki/display/ANTLR3/Python+runtime
# http://www.antlr.org/download/antlr-3.1.3.jar

def main(argv):
    input = InputStream('line from 3, 2 to 7, 4')
    lexer = dsltestLexer(input)
    stream = CommonTokenStream(lexer)
    parser = dsltestParser(stream)
    tree = parser.cmdfile()
    print(tree.toStringTree(recog=parser))

if __name__ == '__main__':
    main(sys.argv)
