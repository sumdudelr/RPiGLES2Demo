AGCC = g++
CXX_FLAG = -g -std=c++11 -Wall -Wextra -pthread
INCLUDE = -I/opt/vc/include
LIBRARY = -L/opt/vc/lib -lbcm_host -lbrcmGLESv2 -lbrcmEGL
TARGET = gles_demo.out

all: $(TARGET)

shader.o: shader.cpp shader.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

gles_demo.out: main.cpp shader.o renderer.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) shader.o $< $(LIBRARY) -o $@
	
clean:
	rm -f $(TARGET) *~
