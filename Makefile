all: plot

plot: run
	gnuplot plot.plt

run: build $(wildcard data.dat)
	mpiexec -np 1 -host localhost:8 ./build

build: $(wildcard *.c *.h)
	mpicc *.c -o build

.PHONY: clean
clean:
	rm -rf build