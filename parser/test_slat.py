#! /usr/bin/env python3
import sys
import glob
from distutils import text_file
from mySlatListener import mySlatListener
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser
from slatListener import slatListener

def main(argv):
    for file in argv[1:]:
        print("File:", file)
        text = open(file).read()
        print()
        print("INPUT:", text)

        input = InputStream(text)
        lexer = slatLexer(input)
        stream = CommonTokenStream(lexer)
        parser = slatParser(stream)
        tree = parser.script()
        #print(tree.toStringTree(recog=parser))
        listener = mySlatListener()
        walker = ParseTreeWalker()
        walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
