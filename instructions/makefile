files := $(wildcard *.cpp)

build:
	$(foreach file, $(files), g++ -I ../include $(file) -fPIC -shared -o ../inst/$(file).so &&) echo "build cmds!"

clean:
	rm ../inst/*.so
