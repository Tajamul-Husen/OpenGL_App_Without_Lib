#include <iostream>
#include "glcore.h"

PFNGLENABLEPROC glEnable = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;
PFNGLBLENDFUNCPROC glBlendFunc = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLCREATEBUFFERSPROC glCreateBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLCLEARPROC glClear = nullptr;
PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;

bool LoadExtension(const char *extensionName, void **functionPtr)
{
    *functionPtr = (void *)wglGetProcAddress(extensionName);
    if (*functionPtr == NULL || (*functionPtr == (void *)0x1) || (*functionPtr == (void *)0x2) || (*functionPtr == (void *)0x3) || (*functionPtr == (void *)-1))
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        *functionPtr = (void *)GetProcAddress(module, extensionName);
    };
    return (*functionPtr != NULL);
};

#define STRINGIFY(s) #s
#define LOADEXT(ext)                                                       \
    if ((extensionsLoaded &= LoadExtension(#ext, (void **)&ext)) == false) \
    printf(STRINGIFY(Failed to load extension :\t##ext\n))

bool LoadGLExtensions()
{
    bool extensionsLoaded = true;

    LOADEXT(glEnable);
    LOADEXT(glDebugMessageCallback);
    LOADEXT(glCreateShader);
    LOADEXT(glShaderSource);
    LOADEXT(glCompileShader);
    LOADEXT(glCreateProgram);
    LOADEXT(glAttachShader);
    LOADEXT(glLinkProgram);
    LOADEXT(glDeleteProgram);
    LOADEXT(glDeleteShader);
    LOADEXT(glDetachShader);
    LOADEXT(glGetShaderiv);
    LOADEXT(glGetProgramiv);
    LOADEXT(glGetShaderInfoLog);
    LOADEXT(glGetProgramInfoLog);
    LOADEXT(glCreateVertexArrays);
    LOADEXT(glBindVertexArray);
    LOADEXT(glCreateBuffers);
    LOADEXT(glBindBuffer);
    LOADEXT(glBufferData);
    LOADEXT(glEnableVertexAttribArray);
    LOADEXT(glVertexAttribPointer);
    LOADEXT(glDeleteVertexArrays);
    LOADEXT(glDeleteBuffers);
    LOADEXT(glClear);
    LOADEXT(glClearColor);
    LOADEXT(glViewport);
    LOADEXT(glUseProgram);
    LOADEXT(glGetUniformLocation);
    LOADEXT(glUniformMatrix4fv);
    LOADEXT(glUniform4f);
    LOADEXT(glDrawElements);
    LOADEXT(glDrawArrays);

    return extensionsLoaded;
};