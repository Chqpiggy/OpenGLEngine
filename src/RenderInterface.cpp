#include "Define.h"
#include "RenderInterface.h"
#include "Manager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//C����ַ��� �ִ�OpenGL��Ҫ������������һ�������һ��Ƭ����ɫ��
//GLSL(OpenGL Shading Language)��д������ɫ��
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
"}\0";
//GLSL(OpenGL Shading Language)��дƬ����ɫ��
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
    glfwWindowHint(GLFW_SAMPLES, 2); // 2�����ز���
    // glfw window creation ����һ�����ڶ��� SCR_WIDTH(��) SCR_HEIGHT(��) "LearnOpenGL"(����)
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // ֪ͨGLFW�����Ǵ��ڵ�����������Ϊ��ǰ�̵߳���������
    glfwMakeContextCurrent(window);

    // ���ǻ���Ҫע���������������GLFW����ϣ��ÿ�����ڵ�����С��ʱ��������������
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers GLAD����������OpenGL�ĺ���ָ��ģ������ڵ����κ�OpenGL�ĺ���֮ǰ������Ҫ��ʼ��GLAD��
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // �����ͱ�����ɫ����
    // ------------------------------------
    // ������ɫ
    // ����Ҫ�����Ǵ���һ����ɫ������ע�⻹����ID������ ���ݸ�����glCreateShader()�Ĳ�����GL_VERTEX_SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // ���ŰѶ�����ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ���������
    /*glShaderSource()������Ҫ�������ɫ��������Ϊ��һ��������
        �ڶ�����ָ���˴��ݵ�Դ���ַ�������������ֻ��һ����
        �����������Ƕ�����ɫ��������Դ�룬
        ���ĸ���������������ΪNULL��*/
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // ��鶥����ɫ���������
    /*����ܻ�ϣ������ڵ���glCompileShader������Ƿ�ɹ��ˣ�
    ���û�ɹ��Ļ����㻹��ϣ��֪��������ʲô������������޸����ǡ�
    ������ʱ�������ͨ�����´�����ʵ��*/
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Ƭ����ɫ��
    //����Ҫ�����Ǵ���һ����ɫ������ע�⻹����ID������ ���ݸ�����glCreateShader()�Ĳ�����GL_FRAGMENT_SHADER
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // ���Ű�Ƭ����ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ���������
       /*glShaderSource()������Ҫ�������ɫ��������Ϊ��һ��������
        �ڶ�����ָ���˴��ݵ�Դ���ַ�������������ֻ��һ����
        ������������Ƭ����ɫ��������Դ�룬
        ���ĸ���������������ΪNULL��*/
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // ���Ƭ����ɫ���������
    /*����ܻ�ϣ������ڵ���glCompileShader������Ƿ�ɹ��ˣ�
    ���û�ɹ��Ļ����㻹��ϣ��֪��������ʲô������������޸����ǡ�
    ������ʱ�������ͨ�����´�����ʵ��*/
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
    //�������� ������������������
    //OpenGL����3D������3���ᣨx��y��z���϶�Ϊ - 1.0��1.0�ķ�Χ��ʱ�Ŵ�������
    //��������ν�ı�׼���豸����(Normalized Device Coordinates)��Χ�ڵ�����Ż����ճ�������Ļ�ϣ��������Χ��������궼������ʾ����
    float vertices[] = {
      -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.3f, 0.0f
    };

    unsigned int indices[] = { // ע��������0��ʼ! 
    0, 1, 2, // ��һ��������
    1, 2, 3  // �ڶ���������
    };
    // VBO ���㻺�����(Vertex Buffer Objects, VBO)�����ڴ�
        /*������GPU�ڴ棨ͨ������Ϊ�Դ棩�д���������㡣
        ʹ����Щ�������ĺô������ǿ���һ���Եķ���һ�������ݵ��Կ��ϣ�������ÿ�����㷢��һ�Ρ�
        ��CPU�����ݷ��͵��Կ���Խ���������ֻҪ�������Ƕ�Ҫ���Ծ���һ���Է��;����ܶ�����ݡ�
        �����ݷ������Կ����ڴ��к󣬶�����ɫ���������������ʶ��㣬���Ǹ��ǳ���Ĺ��̡�*/
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    //VBO�ж�һ�޶���ID���������ǿ���ʹ��glGenBuffers������һ������ID����һ��VBO����
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    //ʹ��glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����,���㻺�����VBO�Ļ���������GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //����glBufferData�����������֮ǰ����Ķ������ݸ��Ƶ�������ڴ��� 
    /*��һ��������Ŀ�껺������ͣ����㻺�����ǰ�󶨵�GL_ARRAY_BUFFERĿ���ϡ�
    �ڶ�������ָ���������ݵĴ�С(���ֽ�Ϊ��λ)����һ���򵥵�sizeof������������ݴ�С���С�
    ����������������ϣ�����͵�ʵ�����ݡ�
    ���ĸ�����ָ��������ϣ���Կ���ι������������*/
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

    // render loop ��Ⱦѭ�� ��GLFW�˳�ǰһֱ��������
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

        //�߿�ģʽ
       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // draw our first triangle
        glUseProgram(shaderProgram);//��ɫ�������������ɫ����Ƭ����ɫ��
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 3);//���������Σ��ڶ�������ָ���˶����������ʼ����������������0�����һ������ָ�����Ǵ�����ƶ��ٸ����㣬������3������
        // ʹ�õ�ǰ�󶨵�������������е��������л���
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose: ���
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // ����Ⱦѭ��������������Ҫ��ȷ�ͷ�/ɾ��֮ǰ�ķ����������Դ
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

//ʵ��һЩ�������
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
    //�����ǿ�ʼ��Ⱦ֮ǰ����һ����Ҫ������Ҫ�������Ǳ������OpenGL��Ⱦ���ڵĳߴ��С��
    //���ӿ�(Viewport)������OpenGL��ֻ��֪���������ݴ��ڴ�С��ʾ���ݺ�����
    //glViewport����ǰ�����������ƴ������½ǵ�λ�á��������͵��ĸ�����������Ⱦ���ڵĿ�Ⱥ͸߶�
    glViewport(0, 0, width, height);
}
