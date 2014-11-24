all: rvm.a
rvm.a: rvm.o
	ar rvs rvm.a rvm.o
rvm.o: rvm.cpp
	g++ -c rvm.cpp
clean:
	rm rvm.a rvm.o
