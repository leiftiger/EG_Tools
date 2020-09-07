
# Compiles the entire project

mkdir -p x64
mkdir -p x64/Release

cd editor

cd ../resourcepacker
make clean
echo

cd ../resourceparser
make clean
echo

cd ../interpreter
make clean
echo

cd ../editor
echo "rm -f ./bin/*.o"
rm -f ./bin/*.o
echo "rm -f ../x64/Release/ATN_Editor.out"
rm -f ../x64/Release/ATN_Editor.out
echo

cd ../resourcemerger
make clean
echo

cd ../modmanager
echo "rm -f ./bin/*.o"
rm -f ./bin/*.o
echo "rm -f ../x64/Release/Mod_Manager.out"
rm -f ../x64/Release/Mod_Manager.out
echo

echo "Cleaned all build files!"
