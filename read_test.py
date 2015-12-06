#! /usr/bin/python3

import pyslat
import numpy as np
import matplotlib.pyplot as plt

command_list = {
    'FUNC': 'Create a function: FUNC ID TYPE PARAMS',
    'IM': 'Intensity Measure',
}

def usage(which=None):
    try:
        print(which)
        print(command_list[which])
    except KeyError:
        for (k, v) in command_list.items():
            print(k, ": " , v)

functions = dict()

def line_error(line):
    print("Error in line: ", line)

try:
    while True:
        line = input()
        command = line.split()
        if len(command) > 0:
            cmd = command[0].upper()
            if command[0]=='FUNC':
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

                    if functions.get(id, False):
                        print("Function '", id, "' already in use.")
                    else:
                        flavour = flavours.get(flavour, False)
                        if type:
                            functions[id] = pyslat.factory(flavour, params)
                        else:
                            line_error(line)
                            print("Error creating function")
                            usage("FUNC")
                else:
                    line_error(line)
                    print("Too few arguments")
                    usage("FUNC")
            elif command[0]=='TABLE':
                start, stop, increment = map(lambda s: float(s), command[2].split(":"))
                main_title, x_title, y_title = command[3].split(":")

                try:
                    f = functions[command[1]]
                    print("\n", main_title)
                    print(x_title, ", ", y_title)
                    for x in np.arange(start, stop+increment, increment):
                        print(x, ", ", f.ValueAt(x))
                except KeyError:
                    line_error(line)
                    print("Undefined function")
            elif command[0]=='PLOT':
                try:
                    f = functions[command[1]]
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
            elif command[0]=='IM':
                line_error(line)
                print("intensity measure not implemented")
            else:
                line_error(line)
                printf("Unrecognised command")
                usage(cmd)
            
except EOFError:
    print("Goodbye!")
    
    print(functions.keys())
    
