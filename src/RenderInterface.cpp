#include "Define.h"
#include "RenderInterface.h"
#include "Manager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//C风格字符串 现代OpenGL需要我们至少设置一个顶点和一个片段着色器
//GLSL(OpenGL Shading Language)编写顶点着色器
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
"}\0";
//GLSL(OpenGL Shading Language)编写片段着色器
const char* fragmentShaderSource = "#version 330 core\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vertexColor;\n"
"}\n\0";

RI_API int __stdcall RI_Init(HWND pWnd)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 2); // 2倍多重采样
    // glfw window creation 创建一个窗口对象 SCR_WIDTH(宽) SCR_HEIGHT(高) "LearnOpenGL"(名称)
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 通知GLFW将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 我们还需要注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数：
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD。
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // 构建和编译着色程序
    // ------------------------------------
    // 顶点着色
    // 首先要做的是创建一个着色器对象，注意还是用ID来引用 传递给函数glCreateShader()的参数是GL_VERTEX_SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 接着把顶点着色器源码附加到着色器对象上，然后编译它：
    /*glShaderSource()函数把要编译的着色器对象作为第一个参数。
        第二参数指定了传递的源码字符串数量，这里只有一个。
        第三个参数是顶点着色器真正的源码，
        第四个参数我们先设置为NULL。*/
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查顶点着色器编译错误
    /*你可能会希望检测在调用glCompileShader后编译是否成功了，
    如果没成功的话，你还会希望知道错误是什么，这样你才能修复它们。
    检测编译时错误可以通过以下代码来实现*/
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 片段着色器
    //首先要做的是创建一个着色器对象，注意还是用ID来引用 传递给函数glCreateShader()的参数是GL_FRAGMENT_SHADER
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 接着把片段着色器源码附加到着色器对象上，然后编译它：
       /*glShaderSource()函数把要编译的着色器对象作为第一个参数。
        第二参数指定了传递的源码字符串数量，这里只有一个。
        第三个参数是片段着色器真正的源码，
        第四个参数我们先设置为NULL。*/
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 检查片段着色器编译错误
    /*你可能会希望检测在调用glCompileShader后编译是否成功了，
    如果没成功的话，你还会希望知道错误是什么，这样你才能修复它们。
    检测编译时错误可以通过以下代码来实现*/
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //顶点数据 三角形三个顶点坐标
    //OpenGL仅当3D坐标在3个轴（x、y和z）上都为 - 1.0到1.0的范围内时才处理它。
    //所有在所谓的标准化设备坐标(Normalized Device Coordinates)范围内的坐标才会最终呈现在屏幕上（在这个范围以外的坐标都不会显示）。
    float vertices[] = {
      -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.3f, 0.0f
    };

    unsigned int indices[] = { // 注意索引从0开始! 
    0, 1, 2, // 第一个三角形
    1, 2, 3  // 第二个三角形
    };
    // VBO 顶点缓冲对象(Vertex Buffer Objects, VBO)管理内存
        /*它会在GPU内存（通常被称为显存）中储存大量顶点。
        使用这些缓冲对象的好处是我们可以一次性的发送一大批数据到显卡上，而不是每个顶点发送一次。
        从CPU把数据发送到显卡相对较慢，所以只要可能我们都要尝试尽量一次性发送尽可能多的数据。
        当数据发送至显卡的内存中后，顶点着色器几乎能立即访问顶点，这是个非常快的过程。*/
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    //VBO有独一无二的ID，所以我们可以使用glGenBuffers函数和一个缓冲ID生成一个VBO对象：
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    //使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上,顶点缓冲对象VBO的缓冲类型是GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中 
    /*第一个参数是目标缓冲的类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上。
    第二个参数指定传输数据的大小(以字节为单位)；用一个简单的sizeof计算出顶点数据大小就行。
    第三个参数是我们希望发送的实际数据。
    第四个参数指定了我们希望显卡如何管理给定的数据*/
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop 渲染循环 让GLFW退出前一直保持运行
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //线框模式
       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // draw our first triangle
        glUseProgram(shaderProgram);//着色程序包括顶点着色器和片段着色器
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 3);//绘制三角形，第二个参数指定了顶点数组的起始索引，我们这里填0。最后一个参数指定我们打算绘制多少个顶点，三角形3个顶点
        // 使用当前绑定的索引缓冲对象中的索引进行绘制
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose: 解绑
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

//实现一些输入控制
void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    //在我们开始渲染之前还有一件重要的事情要做，我们必须告诉OpenGL渲染窗口的尺寸大小，
    //即视口(Viewport)，这样OpenGL才只能知道怎样根据窗口大小显示数据和坐标
    //glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度
    glViewport(0, 0, width, height);
}
