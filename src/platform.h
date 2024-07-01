#pragma once

#include "GL/glcorearb.h"
#include "GL/wglext.h"

#include <iostream>

#define LOG_INFO(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define FATAL_ERROR(msg)                                         \
    MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONEXCLAMATION); \
    exit(EXIT_FAILURE);

struct Window
{
    HWND windowHandle;
    HDC deviceContext;
    HGLRC renderingContext;
};

struct WglExtensions
{
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
};

typedef void (*WindowResizeCallbackType)(int, int);
typedef void (*WindowCloseCallbackType)(void);

struct WindowCallbacks
{
    WindowCloseCallbackType close;
    WindowResizeCallbackType resize;
};

static WindowCallbacks windowCallbacks;

namespace Platform
{

    LRESULT CALLBACK WindowProcessCallback(HWND handleWindow, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            if (windowCallbacks.resize)
            {
                windowCallbacks.resize(width, height);
            };
        }
        break;
        case WM_SYSCOMMAND:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        }
        return DefWindowProcW(handleWindow, msg, wParam, lParam);
    };

    bool LoadWGLExtensions(WglExtensions &extensions)
    {
        WNDCLASSW windowClass{};
        windowClass.style = CS_OWNDC;
        windowClass.lpfnWndProc = DefWindowProcW;
        windowClass.lpszClassName = L"Dummy_Window_Class_Name";

        if (!RegisterClassW(&windowClass))
        {
            LOG_ERROR("Failed to register dummy window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        HWND dummyWindow = CreateWindowExW(
            0,
            windowClass.lpszClassName,
            L"Dummy OpenGL Window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            0,
            0);

        if (!dummyWindow)
        {
            LOG_ERROR("Failed to create dummy window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        HDC dummyDc = GetDC(dummyWindow);

        PIXELFORMATDESCRIPTOR pfd{};

        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pixelFormat = ChoosePixelFormat(dummyDc, &pfd);
        if (!pixelFormat)
        {
            LOG_ERROR("Failed to find a suitable pixel format for dummy window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        if (!SetPixelFormat(dummyDc, pixelFormat, &pfd))
        {
            LOG_ERROR("Failed to set the pixel format for dummy window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        HGLRC dummyContext = wglCreateContext(dummyDc);
        if (!dummyContext)
        {
            LOG_ERROR("Failed to create a dummy rendering context!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        if (!wglMakeCurrent(dummyDc, dummyContext))
        {
            LOG_ERROR("Failed to activate dummy rendering context!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return false;
        };

        // Load the required extensions
        extensions.wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        extensions.wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

        wglMakeCurrent(dummyDc, 0);
        wglDeleteContext(dummyContext);
        ReleaseDC(dummyWindow, dummyDc);
        DestroyWindow(dummyWindow);

        return true;
    };

    Window *CreateWindowSurface(const char *title, int width, int height)
    {
        WNDCLASSW windowClass{0};
        windowClass.style = CS_OWNDC; // own device context
        windowClass.lpfnWndProc = &WindowProcessCallback;
        windowClass.hInstance = GetModuleHandle(0);
        windowClass.lpszClassName = L"Window_Class_Name_OpenGL";

        if (!RegisterClassW(&windowClass))
        {
            LOG_ERROR("Failed to register window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        wchar_t *wideTitle = new wchar_t[128];
        MultiByteToWideChar(CP_ACP, 0, title, -1, wideTitle, 128);

        HWND windowHandle = NULL;

        windowHandle = CreateWindowExW(
            WS_EX_APPWINDOW,
            windowClass.lpszClassName,
            wideTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            width,
            height,
            0,
            0,
            windowClass.hInstance,
            0);

        if (!windowHandle)
        {
            LOG_ERROR("Failed to create window!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        delete[] wideTitle;

        HDC deviceContext = GetDC(windowHandle);
        if (!deviceContext)
        {
            LOG_ERROR("Failed to get device context!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        WglExtensions wglExtensions;

        if (!LoadWGLExtensions(wglExtensions))
        {
            LOG_ERROR("Failed to load wgl extensions!");
            return nullptr;
        };

        // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
        int pixel_format_attribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0};

        int pixel_format;
        UINT num_formats;

        wglExtensions.wglChoosePixelFormatARB(deviceContext, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

        if (!num_formats)
        {
            LOG_ERROR("Failed to choose the pixel format!");
            return nullptr;
        };

        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(deviceContext, pixel_format, sizeof(pfd), &pfd);
        if (!SetPixelFormat(deviceContext, pixel_format, &pfd))
        {
            LOG_ERROR("Failed to set the pixel format!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        // Specify that we want to create an OpenGL 4.6 core profile context
        int glAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            4,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            6,
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        HGLRC glContext = wglExtensions.wglCreateContextAttribsARB(deviceContext, 0, glAttribs);
        if (!glContext)
        {
            LOG_ERROR("Failed to create rendering context!");
            return nullptr;
        };

        if (!wglMakeCurrent(deviceContext, glContext))
        {
            LOG_ERROR("Failed to activate rendering context!");
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        ShowWindow(windowHandle, SW_SHOWDEFAULT);
        UpdateWindow(windowHandle);

        Window *window = new Window();
        window->windowHandle = windowHandle;
        window->deviceContext = deviceContext;
        window->renderingContext = glContext;

        return window;
    };

    void DestroyWindowSurface(Window *window)
    {
        if (window->renderingContext)
            wglDeleteContext(window->renderingContext);
        if (window->deviceContext)
            ReleaseDC(window->windowHandle, window->deviceContext);
        if (window->windowHandle)
            DestroyWindow(window->windowHandle);
    };

    void PollEvents()
    {
        MSG msg;
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_QUIT:
                // close app.
                if (windowCallbacks.close)
                {
                    windowCallbacks.close();
                };
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
                break;
            };
        };
    };

    void WindowResizeCallback(void (*callback)(int width, int height))
    {
        windowCallbacks.resize = callback;
    };

    void WindowCloseCallback(void (*callback)(void))
    {
        windowCallbacks.close = callback;
    };

    void SwapBuffers(Window *window)
    {
        SwapBuffers(window->deviceContext);
    };

    void *ReadSrcFile(const char *fileName)
    {
        DWORD bytesRead;
        void *fileBuffer = nullptr;

        HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR("Failed to create a file handle for %s!", fileName);
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(fileHandle, &fileSize))
        {
            LOG_ERROR("Failed to get file size for %s!", fileName);
            LOG_ERROR("GetLastError=%lu", GetLastError());
            return nullptr;
        };

        LONGLONG fileContentsSize = fileSize.QuadPart;
        fileBuffer = malloc(fileContentsSize + 1);
        ((char *)fileBuffer)[fileContentsSize] = '\0';

        if (!ReadFile(fileHandle, fileBuffer, fileContentsSize, &bytesRead, 0) && bytesRead == fileContentsSize)
        {
            LOG_ERROR("Unable to read from file %s!", fileName);
            LOG_ERROR("GetLastError=%lu", GetLastError());
            CloseHandle(fileHandle);
            return nullptr;
        }
        else if (bytesRead == 0)
        {
            LOG_INFO("No data read from file %s!", fileName);
        };

        LOG_INFO("Data read from file '%s' (%lu bytes)", fileName, bytesRead);

        CloseHandle(fileHandle);

        return fileBuffer;
    };
};