#include "label.hpp"

#include "ellipsoid.hpp"

#include "stb_truetype.h"

#include <iostream>
#include <sstream>

Label::Label() { }

Label::~Label() {
    glDeleteTextures(1, &_tex);
    glDeleteBuffers(2, &_buffers[0]);
}

void Label::init(Camera* camera, std::vector<Point> points) {
    _camera = camera;
    
    // Generate bitmap texture for font
    unsigned char temp_bitmap[512*512];
    stbtt_bakedchar cdata[96];
    std::ifstream ttf("/usr/share/fonts/truetype/quicksand/Quicksand-Regular.ttf");
    std::ostringstream ss;
    ss << ttf.rdbuf();
    std::string str = ss.str();
    stbtt_BakeFontBitmap((unsigned char*)str.c_str(), 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata);
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    
    // Process points
    for (const auto& point : points) {
        P p;
        p.position = GeodeticToCart(point.coordinates);
        p.numCharAttrib = 0;
        
        // Keep track of the overall bounds
        float minX = 0.0f, minY = 0.0f, maxX = 0.0f, maxY = 0.0f;
        float x = 0.0f, y = 0.0f;
        const char* text = &point.name[0];
        while (*text) {
            if (*text >= 32 && *text < 128) {
                /* This stbtt function is producing quads which aren't supported in
                 * OpenGL ES. The workaround for this is to generate the correct
                 * indices to construct the quads out of triangles. The function
                 * produces x,y coordinates with origin at top left and units of
                 * pixels. The rendering will be done with orthographic projection
                 * so that the text is always the same size.
                 */
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(cdata, 512, 512, *text-32, &x, &y, &q, 1);
                 
                minX = std::min(minX, q.x0);
                minY = std::min(minY, q.y0);
                maxX = std::max(maxX, q.x1);
                maxY = std::max(maxY, q.y1);
                 
                Attrib tl = {glm::vec2(q.x0, q.y0), glm::vec2(q.s0, q.t0)};
                Attrib tr = {glm::vec2(q.x1, q.y0), glm::vec2(q.s1, q.t0)};
                Attrib br = {glm::vec2(q.x1, q.y1), glm::vec2(q.s1, q.t1)};
                Attrib bl = {glm::vec2(q.x0, q.y1), glm::vec2(q.s0, q.t1)};
                
                // first triangle
                _vertices.push_back(tl);
                _vertices.push_back(tr);
                _vertices.push_back(bl);
                // second triangle
                _vertices.push_back(br);
                _vertices.push_back(bl);
                _vertices.push_back(tr);
                
                // Keep track of the number of elements added to attribute buffer
                p.numCharAttrib += 6;
            }
            ++text;
        }
        float width = (maxX - minX) / 2.0f;
        float height = (maxY - minY) / 2.0f;
        
        // Shift so the middle is 40 px above 0,0
        p.shift = glm::vec2(-width, -height + 40.0f);
        
        // Create a box to surround the text
        Attrib topLeft = {glm::vec2(minX, minY), glm::vec2(0.0f, 0.1f)};
        Attrib topRight = {glm::vec2(maxX, minY), glm::vec2(0.0f, 0.1f)};
        Attrib botRight = {glm::vec2(maxX, maxY), glm::vec2(0.0f, 0.1f)};
        Attrib botLeft = {glm::vec2(minX, maxY), glm::vec2(0.0f, 0.1f)};
        // first triangle
        _vertices.push_back(topLeft);
        _vertices.push_back(topRight);
        _vertices.push_back(botLeft);
        // second triangle
        _vertices.push_back(botRight);
        _vertices.push_back(botLeft);
        _vertices.push_back(topRight);
        
        // Add the point to vector
        _points.push_back(p);
    }
    
    _shader.init("renderables/shaders/labelV.glsl", "renderables/shaders/labelF.glsl");
    
    // Generate buffers
    glGenBuffers(1, &_buffers[0]);
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Attrib)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW);
    
    _vertLoc = glGetAttribLocation(_shader.ID, "Vert");
    _texcLoc = glGetAttribLocation(_shader.ID, "Texc");
}

void Label::render() {
    _shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);
    _shader.setInt("texture", 0);
    
    glm::mat4 projection = _camera->getOrthoMatrix();
    _shader.setMat4("projection", projection);
    
    // Position attribute
    glEnableVertexAttribArray(_vertLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glVertexAttribPointer(_vertLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), (void*)0);
    
    // Texture coord attribute
    glEnableVertexAttribArray(_texcLoc);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glVertexAttribPointer(_texcLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), (void*)sizeof(glm::vec2));
    
    int index = 0;
    for (const auto& p : _points) {
        // Get clip-space coordianates of perspective point
        glm::vec2 pos = _camera->getScreenPoint(p.position);
        
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(pos + p.shift, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        _shader.setMat4("model", model);
        
        _shader.setBool("isBox", false);
        glDrawArrays(GL_TRIANGLES, index, p.numCharAttrib);
        
        _shader.setBool("isBox", true);
        glDrawArrays(GL_TRIANGLES, index + p.numCharAttrib, 6);
        
        index += p.numCharAttrib + 6;
    }
    
    glDisableVertexAttribArray(_vertLoc);
    glDisableVertexAttribArray(_texcLoc);
}
