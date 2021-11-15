objects = main/main.o components/manipulate_string.o components/memory_allocation.o


miniSh: $(objects) 
	cc -o miniSh $(objects)



main/main.o:                    components/manipulate_string.h components/memory_allocation.h
components/manipulate_string.o: components/manipulate_string.h
components/memory_allocation.o: components/memory_allocation.h



.PHONY: clean
clean: 
	rm miniSh $(objects)

 
