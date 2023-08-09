#include "../include/Core/ResUtil.hpp"

std::string ReadFile(std::string location)
{
    std::string contents;
    std::ifstream file(location);
    if(!file)
        std::cout << "Location for File not found " << location << std::endl;

    std::string line;
    while(std::getline(file, line))
    {
        contents += line + "\n";
    }

    file.close();
    return contents;
}

std::string stringRemove(std::string s, std::string del = " ", std::string adder = "")
{
    int start, end = -1*del.size();
    std::string a;
    do {
        start = end + del.size();
        end = s.find(del, start);
        a += s.substr(start, end - start) += adder;
    } while (end != -1);
    return a;
}

void LoadModel(Mesh *mesh, std::string location)
{
    std::ifstream file(location);
    if(!file)
        std::cout << "Location for Mesh not found " << location << std::endl;

    std::string line;
    std::vector<Vertex> vertexes;
    std::vector<glm::vec2> tmpUV;
    std::vector<glm::vec3> tmpNormal;
    std::vector<glm::vec3> tmpVertice;
    std::vector<unsigned int> tmpInd, tmpUVInd, tmpNormalInd;
    Vertex vertex(glm::vec3(0), glm::vec3(0), glm::vec2(0));
    while(std::getline(file, line))
    {
        if(line[0] == 'v' && line[1] != 't' && line[1] != 'n')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tmpVertice.push_back(glm::vec3(x,y,z));
        }
        else if(line[0] == 'v' && line[1] == 't' && line[1] != 'n')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y;
            data >> x;
            data >> y;
            tmpUV.push_back(glm::vec2(x, y));
        }
        else if(line[0] == 'v' && line[1] != 't' && line[1] == 'n')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tmpNormal.push_back(glm::vec3(x, y, z));
        }
        else if(line[0] == 'f')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", " ");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            unsigned int ind, uvind, norind;
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
        }
    }
    for (unsigned int i = 0; i < tmpInd.size(); i++)
    {
        unsigned int indUv = tmpUVInd[i];
        unsigned int indNor = tmpNormalInd[i];
        unsigned int indVert = tmpInd[i];
        mesh->indices.push_back(i);
        vertexes.push_back(vertex);
        vertexes[i].uv = tmpUV[indUv - 1];
        vertexes[i].normal = tmpNormal[indNor - 1]; 
        vertexes[i].position = tmpVertice[indVert - 1];
    }
    mesh->vertexes = vertexes;
    file.close();
}

int parseLine(char* line){
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int GetRamUsage()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}