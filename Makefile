all :
	mkdir -p build
	cd build && cmake .. && make && make test

clean:
	rm -rf build 
