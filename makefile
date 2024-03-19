build:
	g++ -g -I include src/*.cpp -export-dynamic -o VM

ins:
	make -C instructions/ build

all:
	make build
	make ins
