.PHONY: all clean

all:
	mkdir -p build
	cd build && cmake .. && cmake --build . -j

clean:
	rm -rf build
