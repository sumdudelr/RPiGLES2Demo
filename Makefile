AGCC = g++
CXX_FLAG = -std=c++11 -Wall -Wextra -pthread
INCLUDE = -I/opt/vc/include -Igeometry -Irenderables -Irenderer -Ithird_party
LIBRARY = -L/opt/vc/lib -lbcm_host -lbrcmGLESv2 -lbrcmEGL
TARGET = gles_demo.out

all: CXX_FLAG += -O3
all: $(TARGET)

debug: CXX_FLAG += -g -fsanitize=address -fno-omit-frame-pointer -static-libasan
debug: $(TARGET)

renderer.o: renderer/renderer.cpp renderer/renderer.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

shader.o: renderer/shader.cpp renderer/shader.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
ellipsoid.o: geometry/ellipsoid.cpp geometry/ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
camera.o: renderer/camera.cpp renderer/camera.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
tessellator.o: geometry/tessellator.cpp geometry/tessellator.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
third_party.o: third_party/third_party.cpp third_party/stb_image.h third_party/stb_image_resize.h third_party/stb_truetype.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
SGP4.o: third_party/SGP4.cpp third_party/SGP4.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
globe.o: renderables/globe.cpp renderables/globe.hpp renderer/renderer.hpp renderer/shader.hpp geometry/tessellator.hpp renderer/camera.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
lines.o: renderables/lines.cpp renderables/lines.hpp geometry/ellipsoid.hpp renderer/shader.hpp renderer/camera.hpp geometry/ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
points.o: renderables/points.cpp renderables/points.hpp geometry/ellipsoid.hpp renderer/shader.hpp renderer/camera.hpp geometry/ellipsoid.hpp
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@
	
label.o: renderables/label.cpp renderables/label.hpp renderer/shader.hpp renderer/camera.hpp third_party/stb_truetype.h
	$(AGCC) $(CXX_FLAG) $(INCLUDE) $< $(LIBRARY) -c -o $@

gles_demo.out: main.cpp renderer.o camera.o shader.o ellipsoid.o tessellator.o globe.o lines.o points.o label.o third_party.o SGP4.o
	$(AGCC) $(CXX_FLAG) $(INCLUDE) renderer.o shader.o ellipsoid.o camera.o tessellator.o third_party.o globe.o lines.o points.o label.o SGP4.o $< $(LIBRARY) -o $@
	
clean:
	rm -f $(TARGET) *.o *~
