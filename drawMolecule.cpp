#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <unordered_set>
#include <algorithm>
//结合位点
#define INDEX {1, 13, 25, 42, 54, 60}
//占用圈数
#define r 2

#define SCALE 0.8f
#define LONGITUDE 50
#define LATITUDE 50
#define RADIUS 0.1
#define SCR_WIDTH 1000
#define SCR_HEIGHT 1000


//结合位点
std::vector<unsigned int> combineIndex = INDEX;
std::vector<unsigned int> around;
const int neighborList[60][3] = {
	{2, 7, 6},//1
	{1, 3, 9},//2
	{2, 4, 11},//3
	{3, 5, 12},//4
	{4, 6, 15},//5
	{1, 5, 17},//6
	{1, 8, 19},//7
	{7, 9, 20},//8
	{2, 8, 10},//9
	{9, 57, 59},//10
	{3, 29, 59},//11
	{4, 13, 29},//12
	{12, 14, 39},//13
	{13, 15, 28},//14
	{5, 14, 16},//15
	{15, 17, 26},//16
	{6, 16, 18},//17
	{17, 19, 24},//18
	{7, 18, 22},//19
	{8, 21, 56},//20
	{20, 22, 54},//21
	{19, 21, 23},//22
	{22, 24, 52},//23
	{18, 23, 25},//24
	{24, 26, 50},//25
	{16, 25, 27},//26
	{26, 28, 49},//27
	{14, 27, 38},//28
	{11, 12, 58},//29
	{31, 37, 60},//30
	{30, 32, 40},//31
	{31, 33, 41},//32
	{32, 34, 44},//33
	{33, 46, 60},//34
	{36, 48, 60},//35
	{35, 37, 49},//36
	{30, 36, 38},//37
	{28, 37, 39},//38
	{13, 38, 40},//39
	{31, 39, 58},//40
	{32, 42, 58},//41
	{41, 43, 59},//42
	{42, 44, 57},//43
	{33, 43, 45},//44
	{44, 46, 55},//45
	{34, 45, 47},//46
	{46, 48, 53},//47
	{47, 35, 51},//48
	{27, 36, 50},//49
	{25, 49, 51},//50
	{48, 50, 52},//51
	{23, 51, 53},//52
	{47, 52, 54},//53
	{21, 53, 55},//54
	{45, 54, 56},//55
	{20, 55, 57},//56
	{10, 43 ,56},//57
	{29, 40, 41},//58
	{10, 11, 42},//59
	{30, 34, 35}//60
};
//结合位点周围占用
void getAroundIndex(){
    int i = 0, j = 0, k = 0;
    for(k = 0; k < combineIndex.size(); k++){
        int start = around.size();
        std::unordered_set<int> finded;
        finded.insert(combineIndex[k] - 1);
        for (i = 0; i < r; i++) {
            if(i == 0){
                finded.insert(neighborList[combineIndex[k] - 1][0] - 1);
                finded.insert(neighborList[combineIndex[k] - 1][1] - 1);
                finded.insert(neighborList[combineIndex[k] - 1][2] - 1);
                around.push_back(neighborList[combineIndex[k] - 1][0] - 1);
                around.push_back(neighborList[combineIndex[k] - 1][1] - 1);
                around.push_back(neighborList[combineIndex[k] - 1][2] - 1);
                continue;
            }
            int end = around.size();
            for (; start < end; start++) {
                for (j = 0; j < 3; j++) {
                    if (finded.find(neighborList[around[start]][j] - 1) == finded.end()) {
                        finded.insert(neighborList[around[start]][j] - 1);
                        around.push_back(neighborList[around[start]][j] - 1);
                    }
                }
            }
        }
    }
    
}



//相机设置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//鼠标控制相机欧拉角
bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

//时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;


float max(float x, float y) {
	return x > y ? x : y;
}
//处理log文件
float fileProcess(std::string fileName, std::vector<float>& points, std::vector<unsigned int>& neighbors) {
	std::fstream in(fileName);
	std::string line;
	int i = 0, j = 0;
	float maximum = 0;
	while (std::getline(in, line)) {
		if (line.find("Optimized Parameters") != line.npos) {
			neighbors.clear();
			std::getline(in, line);
			std::getline(in, line);
			std::getline(in, line);
			std::getline(in, line);
			while (std::getline(in, line) && line.find("! R") != line.npos) {
				for (i = 0; line[i + 11] != ','; i++);
				neighbors.push_back(stoi(line.substr(11, i)) - 1);
				for (j = 0; line[i + 12 + j] != ' '; j++);
				neighbors.push_back(stoi(line.substr(12 + i, j)) - 1);
			}
		}
		if (line.find("Standard orientation") != line.npos) {
			points.clear();
			std::getline(in, line);
			std::getline(in, line);
			std::getline(in, line);
			std::getline(in, line);
			while (std::getline(in, line) && line.find("--") == line.npos) {
				line += "\n";
				for (i = 0; line[i + 38] != ' '; i++);
				if (line[37] == '-')
					points.push_back(stod(line.substr(37, i + 1)));
				else
					points.push_back(stod(line.substr(38, i)));
				maximum = max(maximum, abs(points[points.size() - 1]));
				for (i = 0; line[i + 50] != ' '; i++);
				if (line[49] == '-')
					points.push_back(stod(line.substr(49, i + 1)));
				else
					points.push_back(stod(line.substr(50, i)));
				maximum = max(maximum, abs(points[points.size() - 1]));
				for (i = 0; line[i + 62] != '\n'; i++);
				if (line[61] == '-')
					points.push_back(stod(line.substr(61, i + 1)));
				else
					points.push_back(stod(line.substr(62, i)));
				maximum = max(maximum, abs(points[points.size() - 1]));
			}
		}
	}
	in.close();
	for (i = 0; i < points.size(); i++) {
		points[i] = points[i] / maximum * SCALE;
	}
	return maximum;
}


//球上取点
glm::vec3 getPoint(GLfloat u, GLfloat v) {
	GLfloat x = RADIUS * sin(M_PI * u) * cos(2 * M_PI * v);
	GLfloat y = RADIUS * sin(M_PI * u) * sin(2 * M_PI * v);
	GLfloat z = RADIUS * cos(M_PI * u);
	return glm::vec3(x, y, z);
}

//单个球的顶点和索引数组生成
//longitude 经线个数
//latitude 纬线个数
void createSphere(GLfloat *spherePoints, GLuint *sphereIndices) {
	GLfloat lon_step = 1.0 / LONGITUDE;
	GLfloat lat_step = 1.0 / LATITUDE;
	GLuint offsetPoints = 0;
	GLuint offsetIndices = 0;
	GLuint indice[3] = { 0 };
    for (int lat = 0; lat < LATITUDE; lat++) {
        for (int lon = 0; lon < LONGITUDE; lon++) {
            glm::vec3 point = getPoint(lat_step * lat, lon_step * lon);
            memcpy(spherePoints + offsetPoints, glm::value_ptr(point), 3 * sizeof(GLfloat));
            offsetPoints += 3;
        }
    }
    for (int lat = 0; lat < LATITUDE - 1; lat++) {
        for (int lon = 0; lon < LONGITUDE - 1; lon++) {
            indice[0] = lat * LONGITUDE + lon;
            indice[1] = lat * LONGITUDE + lon + 1;
            indice[2] = (lat + 1) * LONGITUDE + lon + 1;
            memcpy(sphereIndices + offsetIndices, indice, 3 * sizeof(GLuint));
            offsetIndices += 3;
            indice[0] = lat * LONGITUDE + lon;
            indice[1] = (lat + 1) * LONGITUDE + lon + 1;
            indice[2] = (lat + 1) * LONGITUDE + lon;
            memcpy(sphereIndices + offsetIndices, indice, 3 * sizeof(GLuint));
            offsetIndices += 3;
        }
        indice[0] = lat * LONGITUDE + LONGITUDE - 1;
        indice[1] = lat * LONGITUDE;
        indice[2] = (lat + 1) * LONGITUDE;
        memcpy(sphereIndices + offsetIndices, indice, 3 * sizeof(GLuint));
        offsetIndices += 3;
        indice[0] = lat * LONGITUDE + LONGITUDE - 1;
        indice[1] = (lat + 1) * LONGITUDE;
        indice[2] = (lat + 1) * LONGITUDE + LONGITUDE - 1;
        memcpy(sphereIndices + offsetIndices, indice, 3 * sizeof(GLuint));
        offsetIndices += 3;
    }
}

//窗口尺寸变化时，调整视口大小的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

//鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;


    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

//输入回调函数
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) * cameraSpeed;
}

int main(){
    //初始化GLFW
    glfwInit();
    //设置OpenGL主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //设置OpenGL次版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //设置GLFW使用OpenGL核心模式，只使用OpenGL一个子集
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //创建窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawMolecule    X:R  Y:G  Z:B", NULL, NULL);
    glfwSetWindowPos(window, 100, 100);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    //初始化GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //开启深度检测
    glEnable(GL_DEPTH_TEST);
    //线宽
    glLineWidth(5);
    //调整窗口大小时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //捕获鼠标输入
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    //生成球顶点坐标
    GLfloat vertices[LATITUDE * LONGITUDE * 3];
	GLuint indices[2 * LONGITUDE * (LATITUDE - 1) * 3];
	createSphere(vertices, indices);
    //读取log文件
    std::vector<float> centers;
	std::vector<unsigned int> neighbors;
	std::string fileName = "C60+-f-1.log";
	fileProcess(fileName, centers, neighbors);
    
    int num = centers.size() / 3;
    int i = 0;
    int j = 0, k = 0;

    getAroundIndex();
    for(i = 0; i < combineIndex.size(); i++){
        combineIndex[i] -= 1;
    }
    sort(combineIndex.begin(), combineIndex.end());
    sort(around.begin(), around.end());

    //坐标轴绘制
    float axeVertices[36] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        SCALE + 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, SCALE + 0.2f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, SCALE + 0.2f, 0.0f, 0.0f, 1.0f
    };

    unsigned int axeIndices[6] = {
        0, 1,
        2, 3,
        4, 5
    };
    //着色器
    Shader atomShader("atom_shader.vs", "atom_shader.fs");
    Shader lineShader("line_shader.vs", "line_shader.fs");
    Shader axeShader("axe_shader.vs", "axe_shader.fs");
    //处理原子
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    //绑定创建的缓冲
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //将顶点数据复制到缓冲的内存中
    //GL_STATIC_DRAW 数据不会或几乎不会改变
    //GL_DYNAMIC_DRAW 数据会被改变很多
    //GL_STREAM_DRAW 数据每次绘制都会改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //告诉OpenGl如何解析顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //以顶点属性位置值作为参数启用顶点属性
    glEnableVertexAttribArray(0);
    //法向
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //处理键
    unsigned int lineVAO, lineVBO, lineEBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1, &lineEBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
    glBufferData(GL_ARRAY_BUFFER, centers.size() * sizeof(float), &centers[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, neighbors.size() * sizeof(int), &neighbors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //画轴
    unsigned int axeVAO, axeVBO, axeEBO;
    glGenVertexArrays(1, & axeVAO);
    glGenBuffers(1, &axeVBO);
    glGenBuffers(1, &axeEBO);
    glBindVertexArray(axeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axeEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axeVertices), axeVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axeIndices), axeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    glBindVertexArray(0);

    
    //渲染循环，先检查GLFW是否被要求退出
    while(!glfwWindowShouldClose(window)){
        //获取时间和帧间时间差
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        j = 0;
        k = 0;
        //检测输入
        processInput(window);
        //清空屏幕颜色缓冲
        glClearColor(0.4f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        atomShader.use();
        //projection矩阵
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        atomShader.setMat4("projection", projection);

        //view矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        atomShader.setMat4("view", view);

        //原子绘制
        glm::mat4 model;
        for(i = 0; i < num; i++){
            if(j < combineIndex.size() && i == combineIndex[j]){
                atomShader.setVec3("objectColor", 0.0f, 0.2f, 1.0f);
                j++;
            }
            else if(k < around.size() && i == around[k]){
                atomShader.setVec3("objectColor", 1.0f, 0.4f, 0.0f);
                k++;
            }
            else{
                atomShader.setVec3("objectColor", 0.5f, 0.5f, 0.5f);
            }
            //atomShader.setVec3("objectColor", 0.5f, 0.5f, 0.5f);
            atomShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            atomShader.setVec3("lightPos", cameraPos);
            atomShader.setVec3("viewPos", cameraPos);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(centers[3 * i], centers[3 * i + 1], centers[3 * i + 2]));
            atomShader.setMat4("model", model);
            glBindVertexArray(VAO);      
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
        }
        //键绘制
        lineShader.use();
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        glBindVertexArray(lineVAO);
        glDrawElements(GL_LINES, neighbors.size(), GL_UNSIGNED_INT, 0);
        //轴绘制
        axeShader.use();
        axeShader.setMat4("projection", projection);
        axeShader.setMat4("view", view);
        //axeShader.setVec3("AxeColor", glm::vec3(1.0, 0.0, 0.0));
        glBindVertexArray(axeVAO);
        glDrawElements(GL_LINES, sizeof(axeIndices), GL_UNSIGNED_INT, 0);


        
        //交换颜色缓冲，用来绘制，并将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        //检查有没有触发事件，更新窗口状态，调用对应回调函数
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    //清理资源，退出程序
    glfwTerminate();
    return 0;
}