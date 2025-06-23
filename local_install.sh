echo "->Compiling XYZ File Builder..."
g++ ./include/build_xyz.cpp
mv ./a.out ./include/build_xyz
echo "->Compiling Main Program..."
g++ ./run.cpp
mv ./a.out ./Gate2D
echo "--->Install Complete!"
