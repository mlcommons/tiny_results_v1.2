
if [[ ${CC} == "" ]] || [[ ${CXX} == "" ]]; then
    echo "cannot find riscv compiler"
    exit
fi

if [[ -d build ]]; then
    rm -r build
fi
touch onnc_runtime.h arm_math_types.h
mkdir build && cd build
cmake ..
make -j2
