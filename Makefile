AGCC = g++
CXX_FLAG = -std=c++11 -Wall -Wextra -pthread
INCLUDE = -I/opt/vc/include
LIBRARY = -L/opt/vc/lib -lbcm_host -lbrcmGLESv2 -lbrcmEGL
TARGET = gles_demo.out

all: CXX_FLAG += -O3
all: $(TARGET)

debug: CXX_FLAG += -g -fsanitize=address -fno-omit-frame-pointer -static-libasan
debug: $(TARGET)

renderer.o: renderer.cpp renderer.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

shader.o: shader.cpp shader.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
tessellator.o: tessellator.cpp tessellator.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
third_party.o: third_party.cpp stb_image.h stb_image_resize.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

gles_demo.out: main.cpp shader.o renderer.o tessellator.o third_party.o
	$(AGCC) $(CXX_FLAG) $(INCLUDE) renderer.o shader.o tessellator.o third_party.o $< $(LIBRARY) -o $@
	
clean:
	rm -f $(TARGET) *.o *~
