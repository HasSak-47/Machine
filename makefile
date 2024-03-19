build:
	g++ -g -I include src/*.cpp -export-dynamic -o VM

clean:
	rm VM

ins:
	make -C instructions/ build

ins_clean:
	make -C instructions/ clean

all:
	make build
	make ins
