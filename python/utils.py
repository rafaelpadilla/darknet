#!/usr/bin/python
import sys
import argparse
import os
import glob


# Validate mandatory args
def ValidateMandatoryArgs(arg, argName, errors):
    if arg is None:
        errors.append('argument %s: required argument' % argName)
    else:
        return True


def ValidatePath(arg, nameArg, errors):
    if arg is None:
        errors.append('%s argument: invalid directory' % nameArg)
    elif os.path.isdir(arg) is False:
        errors.append('%s argument: directory does not exist \'%s\'' % (nameArg, arg))
    # if current path
    if arg == '.':
        return os.path.dirname(os.path.abspath(__file__))
    return arg


def ValidateOutput(arg, nameArg, errors):
    if arg is not None:
        p, f = SplitPathFile(arg)
        if os.path.isdir(p) is False:
            errors.append('%s argument: directory does not exist \'%s\'' % (nameArg, p))
    return arg


def ValidateExtension(arg, nameArg, errors):
    errorMsg = '%s argument: invalid extension\n A valid extension must have 3 characters' % nameArg
    if arg is None:
        errors.append(errorMsg)
        return arg
    arg = arg.replace('.', '')
    if arg == '*':
        return arg
    if len(arg) != 3:
        errors.append(errorMsg)
        return arg
    forbiddenCharacters = '/\:|=?";\[\],^. '
    for c in arg:
        if c in forbiddenCharacters:
            errors.append(errorMsg)
            return arg
    return arg


def ValidateBool(arg, nameArg, errors):
    errorMsg = '%s argument: invalid input\n Valid quiet parameters are [1,True,0,False].' % nameArg
    validParameters = ['1', 'true', '0', 'false']
    if str.lower(arg) not in validParameters:
        errors.append(errorMsg)
        return arg
    if str.lower(arg) == 'true' or str.lower(arg) == '1':
        arg = True
    else:
        arg = False
    return arg


def ValidateFunction(functionName):
    for af in allowedFunctions:
        if functionName in af:
            return ""
    return "Function \'%s\' is not valid" % functionName


def SplitPathFile(fileDataPath):
    idx = fileDataPath.rfind('/')
    p = fileDataPath[:idx + 1]  # path
    f = fileDataPath[idx + 1:]  # file
    return p, f


###############################################################################
# Lists files in a folder and can write them in a txt file
###############################################################################
def SetArgv_listfiles(parser, version):
    functionName = 'List Files'
    parser.prog = parser.prog.replace('##function_name##',
                                      functionName + ' %s' % '[listfiles] [lf]')
    parser.add_argument('-v', '--version', action='version', version=functionName + ' %s' % version)
    # Positional arguments
    parser.add_argument('folder', help='folder containing your files to be ' 'listed')
    # Optional arguments:
    parser.add_argument(
        '-out',
        '--output',
        dest='output',
        metavar='',
        help='file where the list of files will be written to.')
    parser.add_argument(
        '-ext',
        '--extension',
        dest='extension',
        metavar='',
        help='the extension of the files to be listed. Default is \'*\'.',
        default='*')
    parser.add_argument(
        '-fp',
        '--fullpath',
        dest='fullPath',
        metavar='',
        help='if present, the file name will be presented with its full path. Default is False.',
        default=False)
    parser.add_argument(
        '-q',
        '--quiet',
        dest='quiet',
        metavar='',
        help='does not present the console output. Default is False.',
        default=False)


def ListFiles(folder, output='', extension='*', fullPath=True, quiet=False):
    '''This method lists files in the folder directory. The files must match the *.extension (*.jpg,
    *.txt, *.*).
    The list of files will be written in the output file. If fullPath is True, it will be written '
    'the full path, otherwise only the name of the file will be written.'''
    if folder == '.':
        folder = ''
    if folder != '' and folder.endswith('/') is False:
        folder = '%s/' % folder
    if extension == '*':
        folder = folder + '*'
    else:
        folder = folder + '*.' + extension
    # create output file if required
    if output is not None:
        fout = open(output, 'w')
    # AQUI! O full path nao está funcionando.
    # Ele não imprime a pasta atual quando é full pathe
    # E também só imprime full path par aas outras pastas, não importa o valor!
    for f in glob.glob(folder):
        # if it is required to write only the name of the file
        if fullPath is False:
            _, f = SplitPathFile(f)
        # if needed to write in a file
        if output is not None:
            fout.write(f + '\n')
        if quiet is False:
            print(f)
    if output is not None:
        fout.close()
        print('\nFile created sucessfully: %s' % output)


############
# BEGINING #
############
descriptionProject = 'This project implements some useful functions to be used during data '\
                     'preparation'
epilog = "Developed by: Rafael Padilla (rafael.padilla@smt.ufrj.br)"

# List of callable functions
allowedFunctions = []
allowedFunctions.append(['listfiles', 'lf'])
# Validate if a callable function was called
errorMsg = ""
if len(sys.argv) >= 2:
    errorMsg = ValidateFunction(str.lower(sys.argv[1]))
if len(sys.argv) == 1 or errorMsg != "":
    print(descriptionProject)
    print(epilog)
    print('')
    if errorMsg != "":
        print(errorMsg)
    print('You need to provide one of the following functions:')
    print('\t[listfiles] or [lf]: List all files in a directory given an extension.')
    print('Example: \'python utils.py listfiles\'')
    print('Use the command \'function -h\' to obtain help for the function')
    sys.exit()

# Create an argparse
parser = argparse.ArgumentParser(
    prog='Useful functions for data preparation\n\nCalled function: ##function_name##\n',
    description=descriptionProject,
    epilog=epilog)

# If function is the lf
if str.lower(sys.argv[1]) in allowedFunctions[0]:
    version = '0.1 (beta)'
    SetArgv_listfiles(parser, version)

# Remove function arg value in order to apply the listfiles validation
calledFunction = sys.argv[1]
sys.argv.remove(calledFunction)
# Parse args
args = parser.parse_args()
# Add function called back
sys.argv = [sys.argv[0], calledFunction] + [i for i in sys.argv[1:]]

# Validate arguments
errors = []
# Folder
folder = ValidatePath(args.folder, 'First positional', errors)
# Extension
extension = ValidateExtension(args.extension, '[-ext, --extension]', errors)
# Quiet
quiet = ValidateBool(str(args.quiet), '[-q, --quiet]', errors)
# Full path
fullPath = ValidateBool(str(args.fullPath), '[-fp, --fullpath]', errors)
# Output
output = ValidateOutput(args.output, '[-out, --output]', errors)

if len(errors) != 0:
    print('Function %s error(s): ' % sys.argv[1])
    [print(e) for e in errors]
    sys.exit()

# If it reached here, call functions
# List file function
if calledFunction in allowedFunctions[0]:
    ListFiles(folder, output=args.output, extension=extension, fullPath=fullPath, quiet=quiet)
