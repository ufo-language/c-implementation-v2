#!/usr/bin/python3

#=====================================================================
# Modify these options as desired
EXEC_NAME='ufo'
EXEC_DIR='bin'
HEADER_DIR='src'
SOURCE_DIR='src'
OBJ_DIR='obj'
C_VERSION='c11'
#COMPILE_OPTS=f"-g -ggdb -Wall -Wextra -Wswitch-enum -pedantic -std={C_VERSION} -fsanitize=address"
COMPILE_OPTS=f"-Os -Wall -Wextra -Wswitch-enum -pedantic -std={C_VERSION}"
#LINK_OPTS="-fsanitize=address -ldl"
LINK_OPTS="-ldl"
#=====================================================================

import os

SourceFiles = {}
HeaderFiles = {}
ObjFiles = {}
NeedsRecompile = []

def normalizeFileName(sourceFileName):
    if sourceFileName.startswith('./'):
        sourceFileName = sourceFileName[2:]
    sourceFileName = sourceFileName.replace('/', '.')
    return sourceFileName

def includesOf(sourceFile):
    includes = []
    file = open(SOURCE_DIR + '/' + sourceFile)
    exclude = False
    comment = False
    for line in file:
        line = line.strip()
        if exclude:
            if line.startswith('#endif'):
                exclude = False
        elif comment:
            if '*/' in line:
                comment = False
        elif(line.startswith('#include "')):
            parts = line.split('"')
            includes.append(parts[1][:-2])  # everything but the .h
        elif line.startswith('#if 0'):
            exclude = True
        elif '/*' in line:
            comment = True
    file.close()
    return includes

def scanTree(dirs):
    for dirName in dirs:
        if os.path.exists(dirName):
            os.chdir(dirName)
            scanTree_aux()
            os.chdir('..')

def scanTree_aux():
        for root, _dirs, fileNames in os.walk('.'):
            if root.startswith("./"):
                root = root[2:]
            for fileName in fileNames:
                if fileName.startswith("."):
                    continue
                (name, ext) = os.path.splitext(fileName)
                longFileName = root + '/' + fileName
                timeStamp = os.path.getmtime(longFileName)
                longExtensionlessFileName = root + '/' + name
                # strip ./ from beginning
                if longExtensionlessFileName.startswith('./'):
                    longExtensionlessFileName = longExtensionlessFileName[2:]

                if fileName.endswith('.h'):
                    HeaderFiles[longExtensionlessFileName] = timeStamp
                elif fileName.endswith('.c'):
                    SourceFiles[longExtensionlessFileName] = timeStamp
                elif fileName.endswith('.o'):
                    ObjFiles[longExtensionlessFileName] = timeStamp

def checkNeedsRecompile():
    for sourceFileName in SourceFiles:
        srcFileTimeStamp = SourceFiles[sourceFileName]
        objFileTimeStamp = ObjFiles.get(normalizeFileName(sourceFileName))
        if (objFileTimeStamp is None or srcFileTimeStamp > objFileTimeStamp):
            NeedsRecompile.append(sourceFileName)
            #print(sourceFileName, "needs recompile because src > obj")
            continue
        includes = includesOf(sourceFileName + '.c')
        for includeFileName in includes:
            includeFileTimeStamp = HeaderFiles.get(includeFileName)
            if includeFileTimeStamp is None:
                raise Exception(f"include file '{includeFileName}.h' not found, from source file {sourceFileName}.c")
            if includeFileTimeStamp > objFileTimeStamp:
                NeedsRecompile.append(sourceFileName)
                #print(sourceFileName, "needs recompile because inc > obj")
                continue

def compileSourceFile(sourceFileName):
    normalizedSourceFileName = normalizeFileName(sourceFileName)
    sourceFileName += '.c'
    cmd = f"gcc -c -o {OBJ_DIR}/{normalizedSourceFileName}.o -I{HEADER_DIR} {COMPILE_OPTS} {SOURCE_DIR}/{sourceFileName}"
    #print( cmd)
    print(sourceFileName)
    os.system(cmd)

def compileSourceFiles():
    if not os.path.exists(OBJ_DIR):
        os.mkdir(OBJ_DIR)
    for sourceFile in NeedsRecompile:
        compileSourceFile(sourceFile)

def makeExecutable():
    if not os.path.exists(EXEC_DIR):
        os.mkdir(EXEC_DIR)
    execName = EXEC_DIR + "/" + EXEC_NAME
    backupName = execName + '-backup'
    if os.path.exists(execName):
        os.rename(execName, backupName)
    cmd = f"gcc -o {execName} {OBJ_DIR}/*.o {LINK_OPTS}"
    print(cmd)
    os.system(cmd)
    if os.path.exists(execName):
        if os.path.exists(backupName):
            os.remove(backupName)
    elif os.path.exists(backupName):
        os.rename(backupName, execName)

def main():
    dirSet = set()
    dirSet.add(HEADER_DIR)
    dirSet.add(SOURCE_DIR)
    dirSet.add(OBJ_DIR)
    dirs = list(dirSet)
    scanTree(dirs)
    try:
        checkNeedsRecompile()
    except Exception as exn:
        print(exn)
        exit(1)
    compileSourceFiles()
    makeExecutable()

if __name__ == '__main__':
    main()
