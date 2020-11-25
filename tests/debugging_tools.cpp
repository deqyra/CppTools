#include "debugging_tools.hpp"

void stringDifference(std::string string1, std::string string2)
{
    Catch::cout() << "String 1 len: " << string1.length() << "; string 2 len: " << string2.length() << std::endl;
    
    int max = std::max(string1.length(), string2.length());
    for (int i = 0; i < max; i++)
    {
        if (string1[i] != string2[i])
        {
            Catch::cout() << "DIFFERENCE SPOTTED at character " << i << ": string 1 is \"" << string1[i] << "\", string 2 is \"" << string2[i] << "\"" << std::endl;
            break;
        }
    }
}