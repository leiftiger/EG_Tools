
# Compiles the entire project

mkdir -p x64
mkdir -p x64/Release

cd editor

cd ../resourcepacker
make
echo

cd ../resourceparser
make
echo

cd ../interpreter
make
echo

cd ../editor
qmake editor.pro
mkdir -p bin
mkdir -p GeneratedFiles
mkdir -p GeneratedFiles/Release
make
mkdir -p bin
mv ATN_Editor.out ../x64/Release/
echo

cd ../resourcemerger
make
echo

cd ../modmanager
qmake modmanager.pro
mkdir -p bin
mkdir -p GeneratedFiles
mkdir -p GeneratedFiles/Release
make
mkdir -p bin
mv Mod_Manager.out ../x64/Release/
echo

echo "Successfully compiled all EG tools!"
