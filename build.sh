
# Compiles the entire project

mkdir -p x64
mkdir -p x64/Release

cd editor

cd ../resourcepacker
make

cd ../resourceparser
make

cd ../interpreter
make

cd ../editor
qmake editor.pro
mkdir -p bin
mkdir -p GeneratedFiles
mkdir -p GeneratedFiles/Release
make
mkdir -p bin
mv ATN_Editor.out ../x64/Release/

cd ../resourcemerger
make

cd ../modmanager
qmake modmanager.pro
mkdir -p bin
mkdir -p GeneratedFiles
mkdir -p GeneratedFiles/Release
make
mkdir -p bin
mv Mod_Manager.out ../x64/Release/

echo "Successfully compiled all EG tools!"
