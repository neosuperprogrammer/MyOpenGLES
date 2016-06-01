//
//  GLView.m
//  EGLTest
//
//  Created by Sangwook Nam on 7/3/14.
//  Copyright (c) 2014 flowgrammer. All rights reserved.
//

#import "GLView.h"
#import <OpenGLES/EAGL.h>
#import "Matrix.hpp"
#import "app.hpp"

@implementation GLView {
    EAGLContext *_context;
    App *_app;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
        eaglLayer.opaque = YES;
        
        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        if (!_context || ![EAGLContext setCurrentContext:_context]) {
            self = nil;
            return nil;
        }
        
        _app = new App(CGRectGetWidth(frame), CGRectGetHeight(frame));
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        _app->initialize([self buildProgram]);
        
        [self drawView:nil];
        
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
    }
    return self;
}

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void)drawView:(CADisplayLink *)displayLink
{
//    NSLog(@"draw");
   
    _app->draw();
    
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}


- (void)dealloc
{
    if ([EAGLContext currentContext] == _context) {
        [EAGLContext setCurrentContext:nil];
    }
    _context = nil;
}

- (GLuint)buildProgram
{
    GLuint program = glCreateProgram();
    
    GLuint vertexShader = [self buildShader:@"vertex" withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self buildShader:@"fragment" withType:GL_FRAGMENT_SHADER];
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    _app->bindAttribute(program);
    
    glLinkProgram(program);
    
    GLint linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar message[256];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        NSString *log = [NSString stringWithUTF8String:message];
        NSLog(@"%@", log);
        exit(1);
    }
    return program;
}

- (GLuint)buildShader:(NSString *)shaderName withType:(GLenum)shaderType
{
    GLuint shader = glCreateShader(shaderType);
    
    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:shaderName ofType:@"glsl"];
    NSError *error;
    NSString *shaderString = [NSString stringWithContentsOfFile:shaderPath encoding:NSUTF8StringEncoding error:&error];
    if (!shaderString) {
        NSLog(@"error loading shader : %@", error.localizedDescription);
        exit(1);
    }
    
    const char * shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringLength = strlen(shaderStringUTF8);
    
    glShaderSource(shader, 1, &shaderStringUTF8, &shaderStringLength);
    glCompileShader(shader);
    
    GLint compileSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar message[256];
        glGetShaderInfoLog(shader, sizeof(message), 0, message);
        NSString *log = [NSString stringWithUTF8String:message];
        NSLog(@"%@", log);
        exit(1);
    }
    
    return shader;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
	for( touch in touches )
	{
		pos = [ touch locationInView:self ];
		
        _app->touchBegan(pos.x,
                         pos.y,
                         [ [ touches anyObject ] tapCount ]);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
	for( touch in touches )
	{
		pos = [ touch locationInView:self ];
		
        _app->touchMoved(pos.x,
                         pos.y,
                         [ [ touches anyObject ] tapCount ]);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch;
	CGPoint pos;
	
	for( touch in touches )
	{
		pos = [ touch locationInView:self ];
		
        _app->touchEnded(pos.x,
                         pos.y,
                         [ [ touches anyObject ] tapCount ]);
	}	
}

@end
