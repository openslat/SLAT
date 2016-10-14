#! /home/mag109/anaconda3/bin/python3

import pyslatcore
print("Imported")
pyslatcore.Init_Caching()
nlh = pyslatcore.factory(pyslatcore.NLH, [1221, 29.8, 62.2])
print(nlh.ValueAt(0))
print(nlh.ValueAt(1))

loglog = pyslatcore.factory(pyslatcore.LOGLOG, [0.1, 3, 6, 9], [0.1, 1, 2, 3])
print(loglog.ValueAt(0.0))
print(loglog.ValueAt(1.5))
print(loglog.ValueAt(3.0))


print("Done")
