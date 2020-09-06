
# Compiles the entire project

mkdir -p x64
mkdir -p x64/Release

cd editor

cd ../resourcepacker
make clean

cd ../resourceparser
make clean

cd ../interpreter
make clean

cd ../editor
make clean
rm -f ./bin/*.o
rm -f ../x64/Release/ATN_Editor.out

cd ../resourcemerger
make clean

cd ../modmanager
make clean
rm -f ./bin/*.o
rm -f ../x64/Release/Mod_Manager.out

echo "Successfully compiled all EG tools!"
