objects = main/main.o components/manipulate_string.o components/memory_allocation.o


miniSh: build $(objects) 
	cc -o build/miniSh $(objects)


build:
	mkdir build

main/main.o:                    components/manipulate_string.h components/memory_allocation.h
components/manipulate_string.o: components/manipulate_string.h
components/memory_allocation.o: components/memory_allocation.h



.PHONY: clean
clean: 
	rm build/miniSh $(objects)
	rmdir build

 
