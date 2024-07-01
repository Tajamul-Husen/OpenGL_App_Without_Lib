#include "platform.h"
#include "glcore.h"

static bool appRunning = true;

unsigned int CompileShader(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    unsigned int program = glCreateProgram();

    // compile vertex shader
    void *vertexShaderSource = Platform::ReadSrcFile(vertexShaderPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc = (const char *)vertexShaderSource;
    glShaderSource(vertexShader, 1, &vsrc, nullptr);
    glCompileShader(vertexShader);

    int isCompiled = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
        LOG_ERROR("Failed to compile vertex shader: %s", infoLog);
        glDeleteShader(vertexShader);
        free(vertexShaderSource);
        return 0;
    };

    // compile fragment shader
    void *fragmentShaderSource = Platform::ReadSrcFile(fragmentShaderPath);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc = (const char *)fragmentShaderSource;
    glShaderSource(fragmentShader, 1, &fsrc, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(fragmentShader, 512, 0, infoLog);
        LOG_ERROR("Failed to compile fragment shader: %s", infoLog);
        glDeleteShader(fragmentShader);
        free(fragmentShaderSource);
        return 0;
    };

    // link both shader into program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE)
    {
        glGetProgramInfoLog(program, 512, 0, infoLog);
        LOG_ERROR("Failed to link program: %s", infoLog);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        return 0;
    };

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexShaderSource);
    free(fragmentShaderSource);

    LOG_INFO("Shaders compiled and linked successfully.");

    return program;
};

int main()
{
    LOG_INFO("Application initialized.");
    Window *window = Platform::CreateWindowSurface("OpenGL Window", 1280, 720);
    if (window == nullptr)
    {
        FATAL_ERROR("Failed to create window surface!");
    };

    if (!LoadGLExtensions())
    {
        FATAL_ERROR("Failed to load GL Extensions!");
    };

    LOG_INFO("GL extensions load successfully.");

    Platform::WindowCloseCallback([]()
                                  { appRunning = false; });

    Platform::WindowResizeCallback([](int width, int height)
                                   { LOG_INFO("[ResizeEvent] width: %d height: %d", width, height); glViewport(0, 0, width, height); });

    unsigned int shaderProgram = CompileShader("assets/shaders/flat_shader.vert.glsl", "assets/shaders/flat_shader.frag.glsl");

    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (appRunning)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        int location = glGetUniformLocation(shaderProgram, "u_Color");
        glUniform4f(location, 1.0f, 0.5f, 0.2f, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Platform::SwapBuffers(window);
        Platform::PollEvents();
    };

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    Platform::DestroyWindowSurface(window);
    LOG_INFO("Application shutdown.");

    return 0;
};