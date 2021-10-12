.PHONY: all debug clean sqlite duckdb
all: debug

clean:
	rm -rf build
	rm -rf sqlite/build
	rm -rf duckdb/build

sqlite:
	mkdir -p sqlite/build/debug && \
	cd sqlite/build/debug && \
	../../configure --prefix=`pwd` --disable-tcl --enable-debug --enable-static && \
	make -j install

duckdb:
	cd duckdb && \
	DISABLE_SANITIZER=1 make debug

debug:
	mkdir -p build/debug && \
	cd build/debug && \
	cmake  -DCMAKE_BUILD_TYPE=Debug ../.. && \
	cmake --build .

