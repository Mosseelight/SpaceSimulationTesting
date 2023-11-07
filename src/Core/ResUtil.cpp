#include "../include/Core/ResUtil.hpp"

std::string ReadFile(std::string location)
{
    std::string contents;
    std::ifstream file(location);
    if(!file)
        DebugLog("Location for File not found " + location);

    std::string line;
    while(std::getline(file, line))
    {
        contents += line + "\n";
    }

    file.close();
    return contents;
}

bool FileExist(std::string location)
{
    std::ifstream check(location);
    if(!check)
    {
        check.close();
        return false;
    }
    else
    {
        check.close();
        return true;
    }
}

void CreateFile(std::string location, std::string name)
{
    std::ifstream check(location + name);
    if(!check)
    {
        std::ofstream file(location + name);
        file.close();
    }
    else
        DebugLog("File " + location + name + " already there");
    check.close();
}

void WriteFile(std::string location, std::string content)
{
    std::ofstream file(location, std::ios::app);
    file << content << std::endl;
    file.close();
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

std::vector<std::string> GetFiles(std::string location)
{
    std::vector<std::string> files;
    for (const auto & entry : std::filesystem::directory_iterator(location))
        files.push_back(stringRemove(entry.path(), location, ""));

    return files;
}

int parseLine(char* line){
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

float GetRamUsage()
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