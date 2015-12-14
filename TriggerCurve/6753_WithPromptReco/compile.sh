input=MakePlots.cpp
output=RunPlots

root_cflags=`root-config --cflags`
root_libs=`root-config --libs`
roofit_libs="-lRooFitCore -lRooFit -lMinuit -lFoam"
pythia_include="-I$PYTHIA8/include"
pythia_lib="-L$PYTHIA8/lib/archive -lpythia8 -llhapdfdummy"
fastjet_cflags=`fastjet-config --cxxflags`
fastjet_libs=`fastjet-config --libs`
cxxflags="-g -fPIC -Wno-deprecated -O -ansi -D_GNU_SOURCE -g -O2"
cxx="-m64"
extrainclude="-I/home/$USER/RootMacros"

AllLibs="$root_libs $fastjet_libs $roofit_libs $pythia_lib"
AllFlags="$root_cflags $fastjet_cflags $cxx $cxxflags"
AllInclude="$pythia_include $extrainclude"

echo "g++ $input $AllLibs $AllFlags $AllInclude -o $output"
g++ $input $AllLibs $AllFlags $AllInclude -o $output



