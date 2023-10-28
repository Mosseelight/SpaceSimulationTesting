#include "../include/Core/Debug.hpp"

std::vector<std::string> logContent;
static DebugShaderStuff stuff;
static std::vector<unsigned int> vaos;
static std::vector<unsigned int> vbos;
static std::vector<unsigned int> drawSizes;

enum types
{
    Line = 2,
    Triangle = 3,
    Cube = 36
};

void DebugLog(std::string content)
{
    logContent.push_back(content);
}

void CreateFileLog(std::string location)
{
    std::time_t timev = time(0);
    tm* localtm = localtime(&timev);
    std::string filename = "log-" + (std::string)asctime(localtm);
    filename += ".txt";
    CreateFile("", location + filename);
    for (unsigned int i = 0; i < logContent.size(); i++)
    {
        WriteFile(location + filename, logContent[i]);
    }
}

void SetNeededDebug(glm::mat4 proj, glm::mat4 view, std::string shaderLoc)
{
    stuff.m_proj = proj;
    stuff.m_view = view;
    stuff.m_shaderLoc = shaderLoc;
}

void DrawDebugLine(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
    unsigned int vbo, vao;
	float points[12];

	points[0] = start.x;
	points[1] = start.y;
	points[2] = start.z;
	points[3] = color.x;
	points[4] = color.y;
	points[5] = color.z;

	points[6] = end.x;
	points[7] = end.y;
	points[8] = end.z;
	points[9] = color.x;
	points[10] = color.y;
	points[11] = color.z;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glBindVertexArray(0);

    vaos.push_back(vao);
    vbos.push_back(vbo);
    drawSizes.push_back(2);
}

void DrawDebugTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color)
{
    unsigned int vbo, vao;
	float points[18];

	points[0] = a.x;
	points[1] = a.y;
	points[2] = a.z;
	points[3] = color.x;
	points[4] = color.y;
	points[5] = color.z;

	points[6] = b.x;
	points[7] = b.y;
	points[8] = b.z;
	points[9] = color.x;
	points[10] = color.y;
	points[11] = color.z;

    points[12] = c.x;
	points[13] = c.y;
	points[14] = c.z;
	points[15] = color.x;
	points[16] = color.y;
	points[17] = color.z;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glBindVertexArray(0);

    vaos.push_back(vao);
    vbos.push_back(vbo);
    drawSizes.push_back(3);
}

void DrawDebugCube(glm::vec3 pos, float scale, glm::vec3 color)
{
    unsigned int vbo, vao;
    float vertices[] = { 
            -scale + pos.x, -scale + pos.y,  scale + pos.z, color.x, color.y, color.z,
            -scale + pos.x,  scale + pos.y,  scale + pos.z, color.x, color.y, color.z,
            -scale + pos.x, -scale + pos.y, -scale + pos.z, color.x, color.y, color.z,
            -scale + pos.x,  scale + pos.y, -scale + pos.z, color.x, color.y, color.z,
             scale + pos.x, -scale + pos.y,  scale + pos.z, color.x, color.y, color.z,
             scale + pos.x,  scale + pos.y,  scale + pos.z, color.x, color.y, color.z,
             scale + pos.x, -scale + pos.y, -scale + pos.z, color.x, color.y, color.z,
             scale + pos.x,  scale + pos.y, -scale + pos.z, color.x, color.y, color.z
        };
    unsigned int indices[] = { 
            1, 2, 0,
            3, 6, 2,
            7, 4, 6,
            5, 0, 4,
            6, 0, 2,
            3, 5, 7,
            1, 3, 2,
            3, 7, 6,
            7, 5, 4,
            5, 1, 0,
            6, 4, 0,
            3, 1, 5
        };
    
	float points[6 * 36];
    for (unsigned int i = 0; i < 36; i++)
    {
        for (unsigned int j = 0; j < 6; j++)
        {
            points[j + 6 * i] = vertices[indices[i] * 6 + j];
        }
    }

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glBindVertexArray(0);

    vaos.push_back(vao);
    vbos.push_back(vbo);
    drawSizes.push_back(36);
}

void DrawDebugItems()
{
    Shader lineShader;
    lineShader.CompileShader(ShaderLoc(ReadFile(stuff.m_shaderLoc + "DebugDrawing.vert"), ReadFile(stuff.m_shaderLoc + "DebugDrawing.frag")));
    for (unsigned int i = 0; i < vaos.size(); i++)
    {
        glUseProgram(lineShader.shader);
        lineShader.setMat4("proj", stuff.m_proj);
        lineShader.setMat4("view", stuff.m_view);
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(vaos[i]);
        if(drawSizes[i] == Line)
            glDrawArrays(GL_LINES, 0, drawSizes[i]);
        if(drawSizes[i] == Cube || drawSizes[i] == Triangle)
            glDrawArrays(GL_TRIANGLES, 0, drawSizes[i]);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
        glDeleteBuffers(1, &vbos[i]);
        glDeleteVertexArrays(1, &vaos[i]);
    }
    vaos.clear();
    vbos.clear();
    drawSizes.clear();
}