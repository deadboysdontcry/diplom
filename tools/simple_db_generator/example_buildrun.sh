#!/bin/bash
clang++ -O3 -std=c++20 -o a.out main.cpp 
./a.out --columns_count=3 --rows_count=10 --max_value=3 --output_path=file.txt
