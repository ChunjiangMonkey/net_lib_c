objects = random_fun.o base.o net_io.o pdg.o 
pdg: $(objects)
	gcc -o pdg $(objects) -lm

base.o: base.h
net_io.o: net_io.h
random_fun.o: random_fun.h
clean:
	del *.exe *.o