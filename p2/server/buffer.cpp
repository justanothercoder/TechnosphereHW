#include "buffer.hpp"
#include <iostream>

void Buffer::add(const std::string& s) 
{
    for (size_t i = 0; i < s.length(); ++i) {
        current += s[i];
        if (s[i] == '\n') {
            text.push(current);
            current.clear();
        }
    }
}

std::string Buffer::get() 
{
    std::string message = text.front();
    text.pop();
    return message;
}

bool Buffer::has_messages() const
{
    return !text.empty();
}
