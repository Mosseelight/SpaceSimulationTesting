#include "../include/Core/ResUtil.hpp"
#include <fstream> 

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