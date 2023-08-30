#include "../include/Core/Debug.hpp"

std::vector<std::string> logContent;

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