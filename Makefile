main:main.cpp function.h
	g++ main.cpp -o ./bin/main

test:test.cpp function.h
	g++ test.cpp -lboost_unit_test_framework -o ./bin/test
