//
//  app.hpp
//  EGLTest
//
//  Created by Sangwook Nam on 7/8/14.
//  Copyright (c) 2014 flowgrammer. All rights reserved.
//

#ifndef EGLTest_app_hpp
#define EGLTest_app_hpp

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

class App {
public:
    App(float width, float height);
    void bindAttribute(GLuint programHandle);
    void initialize(GLuint programHandle);
    void draw();
    virtual ~App() {}
    void touchBegan(float x, float y, unsigned int tapCount);
    void touchMoved(float x, float y, unsigned int tapCount);
    void touchEnded(float x, float y, unsigned int tapCount);
private:
    void applyModelView();
    void applyProjection();
    
    float _width, _height;
    GLuint _program;
};


#endif
