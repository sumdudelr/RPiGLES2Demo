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
	
ellipsoid.o: ellipsoid.cpp ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
camera.o: camera.cpp camera.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
tessellator.o: tessellator.cpp tessellator.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
third_party.o: third_party.cpp stb_image.h stb_image_resize.h stb_truetype.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
globe.o: globe.cpp globe.hpp renderer.hpp shader.hpp tessellator.hpp camera.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
lines.o: lines.cpp lines.hpp ellipsoid.hpp shader.hpp camera.hpp ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
points.o: points.cpp points.hpp ellipsoid.hpp shader.hpp camera.hpp ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
label.o: label.cpp label.hpp shader.hpp camera.hpp stb_truetype.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

gles_demo.out: main.cpp renderer.o camera.o shader.o ellipsoid.o tessellator.o globe.o lines.o points.o label.o third_party.o
	$(AGCC) $(CXX_FLAG) $(INCLUDE) renderer.o shader.o ellipsoid.o camera.o tessellator.o third_party.o globe.o lines.o points.o label.o $< $(LIBRARY) -o $@
	
clean:
	rm -f $(TARGET) *.o *~
