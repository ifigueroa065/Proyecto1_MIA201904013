OBJECTS := main.o
INCLUDES := ../include/*.cpp

ejecutable : $(OBJECTS)
	g++ -o $@ $^ $(INCLUDES)
	rm -f *.o

*.o: %.cpp
	g++ -o $@ -c $^ -I