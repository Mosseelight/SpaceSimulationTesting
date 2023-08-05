#include "../include/Core/ResUtil.hpp"

std::string ReadFile(std::string location)
{
    std::string contents;
    std::ifstream file(location);

    std::string line;
    while(std::getline(file, line))
    {
        contents += line + "\n";
    }

    file.close();
    return contents;
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