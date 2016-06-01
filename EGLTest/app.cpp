//
//  App.cpp
//  EGLTest
//
//  Created by Sangwook Nam on 7/8/14.
//  Copyright (c) 2014 flowgrammer. All rights reserved.
//

#include "app.hpp"
#include "Quaternion.hpp"
#include <stdio.h>
#include <sys/time.h>

typedef struct Vertex_ {
    vec3 Position;
    vec4 Color;
} Vertex;

const Vertex Vertices[] = {
    {{-0.5, -0.866, -0.01}, {1, 0, 0.5f, 1}},
    {{0.5, -0.866, -0.01},  {0, 1, 0.0f, 1}},
    {{0, 1, -0.01},         {0, 0, 1.0f, 1}},
    {{-0.5, -0.866, 0.0}, {0.5f, 0.5f, 0.5f, 1}},
    {{0.5, -0.866, 0.0},  {0.5f, 0.5f, 0.5f, 1}},
    {{0, -0.4f, 0.0},     {0.5f, 0.5f, 0.5f, 1}},
};

const GLubyte indices[] = { 0, 1, 2, 3, 4, 5 };

GLuint frameBuffer, renderBuffer, depthBuffer;
GLuint vertextBuffer, indexBuffer;
GLuint vertexIndex;

App::App(float width, float height) : _width(width), _height(height)
{
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
}

GLuint positionAttr = 3;
GLuint colorAttr = 5;
void App::bindAttribute(GLuint programHandle)
{
    glBindAttribLocation(programHandle, positionAttr, "Position");
    glBindAttribLocation(programHandle, colorAttr, "SourceColor");
}


void App::initialize(GLuint programHandle)
{
    _program = programHandle;
    glUseProgram(_program);
    
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _width, _height);
    
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glViewport(0, 0, _width, _height);
    

    glGenVertexArraysOES(1, &vertexIndex);
    glBindVertexArrayOES(vertexIndex);
    
    glGenBuffers(1, &vertextBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertextBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
//    GLuint positionSlot = glGetAttribLocation(_program, "Position");
//    GLuint colorSlot = glGetAttribLocation(_program, "SourceColor");
    
    glEnableVertexAttribArray(positionAttr);
    glEnableVertexAttribArray(colorAttr);
    
    GLsizei stride = sizeof(Vertex);
    
    //    const GLvoid* pCoords = &Vertices[0].Position.x;
    //    const GLvoid* pColors = &Vertices[0].Color.x;
    
    const GLvoid* colorOffset = (GLvoid*)sizeof(vec3);
    
    glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(colorAttr, 4, GL_FLOAT, GL_FALSE, stride, colorOffset);

    
    glBindVertexArrayOES(0);
    
    
    glDisableVertexAttribArray(positionAttr);
    glDisableVertexAttribArray(colorAttr);
    
    glBindVertexArrayOES(vertexIndex);
}

unsigned int get_milli_time( void )
{
    struct timeval tv;
    
	gettimeofday( &tv, NULL );
    
    return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

void App::draw()
{
//    printf("draw\n");
    
    static unsigned int start = get_milli_time(),
    fps = 0;
    if( get_milli_time() - start >= 1000 ) {
        printf("FPS: %d\n", fps );
        start = get_milli_time();
        fps = 0;
    }
    ++fps;
    
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    applyModelView();
    applyProjection();
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_BYTE, 0);
}

void App::applyProjection()
{
    mat4 mat = mat4::Frustum(45.0, _width/_height, 0.01, 100);
    GLint projectionUniform = glGetUniformLocation(_program, "Projection");
    glUniformMatrix4fv(projectionUniform, 1, 0, mat.Pointer());
}

float _prevX;
float _prevY;

float _offsetX = 0.0;
float _offsetY = 0.0;

void App::applyModelView()
{
    if (_offsetX > 360) {
        _offsetX = 0;
    }
    if (_offsetX < -360) {
        _offsetX = 0;
    }
//    mat4 rotation1 = mat4::Rotate(-_offsetX, vec3(0, 1, 0));
    
    mat4 rotationY(Quaternion::CreateFromAxisAngle(vec3(0.0, 1.0, 0.0), _offsetX*(3.14/180)).ToMatrix());
    mat4 rotationX(Quaternion::CreateFromAxisAngle(vec3(1.0, 0.0, 0.0), _offsetY*(3.14/180)).ToMatrix());
//    mat4 rotation2 = mat4::Rotate(-_offsetY, vec3(1, 0, 0));
    mat4 lookat = mat4::LookAt(vec3(0.0, 0.0, 2.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    mat4 mat = rotationY * rotationX * lookat;
    
    GLint modelviewUniform = glGetUniformLocation(_program, "ModelView");
    glUniformMatrix4fv(modelviewUniform, 1, 0, mat.Pointer());
}

void App::touchBegan(float x, float y, unsigned int tapCount)
{
    _prevX = x;
    _prevY = y;
}

void App::touchMoved(float x, float y, unsigned int tapCount)
{
    _offsetX = x - _prevX;
    _offsetY = y - _prevY;
    
//    _prevX = x;
//    _prevY = y;
}

void App::touchEnded(float x, float y, unsigned int tapCount)
{
    
}

