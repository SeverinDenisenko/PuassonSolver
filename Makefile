all: plot

plot: run $(wildcard plot.plt)
	gnuplot plot.plt

run: build $(wildcard data.dat)
	mpiexec -np 5 -host localhost:8 ./build

build: $(wildcard *.c *.h)
	mpicc *.c -o build

.PHONY: clean
clean:
	rm -rf build