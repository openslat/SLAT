#! /usr/bin/python3

import sys
import pyslat
import numpy as np
import matplotlib.pyplot as plt

command_list = {
    'DFUNC': 'Create a deterministic function: FUNC ID TYPE PARAMS',
    'DTABLE': 'Print a table of values: DTABLE <DFUNC ID> <min:max:step> <x label:y label>',
    'DPLOT': 'Plot a function: DPLOT <DFUNC ID> <min:max:step> <Title:X label:Y label> [file]',
    'PFUNC': 'Create a probabilistic functioN: PFUNC ID TYPE MU_FUNCTION SIGMA_FUNCTION',
    'IM': 'Intensity Measure',
}

def usage(which=None):
    try:
        print(which)
        print(command_list[which])
    except KeyError:
        for (k, v) in command_list.items():
            print(k, ": " , v)

deter_funcs = dict()
prob_funcs = dict()

def func_in_use(f):
    deter_funcs.get(f, False) or prob_funcs.get(f, False)


def line_error(line):
    print("Error in line: ", line)

try:
    while True:
        line = input()
        print("LINE: ", line)
        if len(line) > 0 and line[0] == '#':
            continue
        
        command = line.split()
        if len(command) > 0:
            cmd = command[0].upper()
            if command[0]=='DFUNC':
                if len(command) > 4:
                    flavours = {
                        'NLH': pyslat.FUNCTION_TYPE.NLH,
                        'PLC': pyslat.FUNCTION_TYPE.PLC,
                        #'LIN': pyslat.FUNCTION_TYPE.LIN,
                        #'LOGLOG': pyslat.FUNCTION_TYPE.LOGLOG
                    }

                    id = command[1]
                    flavour = command[2]
                    params = list(map(lambda s: float(s), command[3:]))

                    if func_in_use(id):
                        raise ValueError("Function {0} already in use.".format(id))

                    flavour = flavours.get(flavour, False)
                    if type:
                        deter_funcs[id] = pyslat.factory(flavour, params)
                    else:
                        line_error(line)
                        print("Error creating function")
                        usage("FUNC")
                else:
                    line_error(line)
                    print("Too few arguments")
                    usage("FUNC")
            elif command[0]=='PFUNC':
                if len(command) == 5:
                    id = command[1]
                    flavour = command[2]
                    mu_func = command[3]
                    sigma_func = command[4]

                    if func_in_use(id):
                        raise ValueError("Function {0} already in use.".format(id))

                    if flavour!="LOGN":
                        raise ValueError("Unrecognised function type: {0}".format(flavour))

                    if not deter_funcs.get(mu_func, False):
                        raise ValueError("Unrecognized mu function: {0}".format(mu_func))

                    if not deter_funcs.get(sigma_func, False):
                        raise ValueError("Unrecognized sigma function: {0}".format(
                            sigma_func))

                    prob_funcs[id] = pyslat.MakeLogNormalProbabilisticFunction(
                        deter_funcs[mu_func],
                        deter_funcs[sigma_func])
                else:
                    line_error(line)
                    print("Wrong number of few arguments")
                    usage("PFUNC")
            elif command[0]=='DTABLE':
                start, stop, increment = map(lambda s: float(s), command[2].split(":"))
                main_title, x_title, y_title = command[3].split(":")

                try:
                    f = deter_funcs[command[1]]
                    if len(command) > 4:
                        file = open(command[4], "w")
                    else:
                        file = sys.stdout
                    file.write("\n{0}\n".format(main_title))
                    file.write("{0}, {1}\n".format(x_title, y_title))
                    for x in np.arange(start, stop+increment, increment):
                        file.write("{0}, {1}\n".format(x, f.ValueAt(x)))
                except KeyError:
                    line_error(line)
                    print("Undefined function")
                    
                    if len(command > 4):
                        file.close()
            elif command[0]=='DPLOT':
                try:
                    f = deter_funcs[command[1]]
                    start, stop, increment = map(lambda s: float(s), command[3].split(":"))
                    x_data = np.arange(start, stop+increment, increment)
                    y_data = list(map(f.ValueAt, x_data))
                    
                    if command[2]=="LIN":
                        plt.plot(x_data, y_data)
                    elif command[2]=="LOG":
                        plt.loglog(x_data, y_data)
                    else:
                        raise ValueError(' '.join(['Unrecognised plot type:', command[2]])) 

                    main_title, x_title, y_title = command[4].split(":")
                    plt.title(main_title)
                    plt.xlabel(x_title)
                    plt.ylabel(y_title)
                    
                    if len(command) > 5:
                        plt.savefig(command[5])
                    else:
                        plt.show()
                except KeyError:
                    line_error(line)
                    print("Undefined function")
            elif command[0]=='PTABLE':
                start, stop, increment = map(lambda s: float(s), command[2].split(":"))
                pcts = list(map(lambda s:float(s), command[3].split(",")))
                print(pcts)
                main_title, x_title, y_title = command[4].split(":")

                try:
                    f = prob_funcs[command[1]]
                    if len(command) > 5:
                        file = open(command[5], "w")
                    else:
                        file = sys.stdout
                    file.write("{0}\n".format(main_title))
                    title_line = x_title
                    for p in pcts:
                        title_line = "{0}, {1} [{2}%]".format(title_line, y_title, p)
                    file.write("{0}\n".format(title_line))

                    for x in np.arange(start, stop+increment, increment):
                        this_line = "{0}".format(x)
                        for p in pcts:
                            this_line = "{0}, {1}".format(this_line,
                                                          f.X_at_exceedence(x, p/100.))
                        file.write("{0}\n".format(this_line))
                except KeyError:
                    line_error(line)
                    print("Undefined function")
            elif command[0]=='IM':
                line_error(line)
                print("intensity measure not implemented")
            else:
                line_error(line)
                print("Unrecognised command")
                usage(cmd)
            
except EOFError:
    print("Goodbye!")

    print("Deterministic functions: ", deter_funcs.keys())
    print("Probabilistic functions: ", prob_funcs.keys())
    
    
