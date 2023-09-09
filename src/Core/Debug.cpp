#include "../include/Core/Debug.hpp"

std::vector<std::string> logContent;
static DebugShaderStuff stuff;
static std::vector<unsigned int> vaos;
static std::vector<unsigned int> vbos;

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

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

    vaos.push_back(vao);
    vbos.push_back(vbo);
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
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
        glDeleteBuffers(1, &vbos[i]);
        glDeleteVertexArrays(1, &vaos[i]);
    }
    std::cout << vaos.size() << std::endl;
    vaos.clear();
    vbos.clear();
}