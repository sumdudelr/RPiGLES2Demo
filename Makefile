AGCC = g++
CXX_FLAG = -std=c++11 -Wall -Wextra -pthread
INCLUDE = -I/opt/vc/include
LIBRARY = -L/opt/vc/lib -lbcm_host -lbrcmGLESv2 -lbrcmEGL
TARGET = gles_demo.out

all: CXX_FLAG += -O3
all: $(TARGET)

debug: CXX_FLAG += -g -fsanitize=address -fno-omit-frame-pointer -static-libasan
debug: $(TARGET)

shader.o: shader.cpp shader.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

gles_demo.out: main.cpp shader.o renderer.hpp tessellator.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) shader.o $< $(LIBRARY) -o $@
	
clean:
	rm -f $(TARGET) *.o *~
