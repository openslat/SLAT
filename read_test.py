#! /usr/bin/python3

import pyslat

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



try:
    while True:
        command = input().split()
        if len(command) > 0:
            cmd = command[0].upper()
            if command[0]=='FUNC':
                if len(command) > 4:
                    flavours = {
                        'NLH': pyslat.FUNCTION_TYPE.NLH,
                        'PLC': pyslat.FUNCTION_TYPE.PLC,
                        'LIN': pyslat.FUNCTION_TYPE.LIN,
                        'LOGLOG': pyslat.FUNCTION_TYPE.LOGLOG
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
                            usage("FUNC")
                else:
                    usage("FUNC")
            elif command[0]=='IM':
                print("intensity measure")
            else:
                usage(cmd)
        else:
            usage()
            
except EOFError:
    print("Goodbye!")
    
    print(functions.keys())
    
