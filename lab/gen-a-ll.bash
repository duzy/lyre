#!/bin/bash
if ! which clang++ ; then
    PATH="/open/llvm/Debug+Asserts/bin:$PATH"
fi
rm -f a.bc
clang++ -emit-llvm -c a.cc
rm -f a.ll
llvm-dis a.bc
