#pragma once

#include <queue>
#include <string>

class Buffer
{
public:
    void add(const std::string& s);
    std::string get();

    bool has_messages() const;
    
private:
    std::string current;
    std::queue<std::string> text;
};
