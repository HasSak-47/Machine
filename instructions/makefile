files := $(wildcard *.cpp)

build_mm:
	g++ -I ../include mov_mm.cpp -fPIC -shared -o ../inst/mov_mm.cpp.so

build:
	$(foreach file, $(files), g++ -I ../include $(file) -fPIC -shared -o ../inst/$(file).so &&) echo "build cmds!"

clean:
	rm ../inst/*.so
