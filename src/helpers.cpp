#include "helpers.hpp"
#include <sstream>

// Helper function to remove whitespaces in a string
string removeWhitespaces(const string str)
{
    string result;
    for (char ch : str)
    {
        if (!isspace(static_cast<unsigned char>(ch)))
        {
            result.push_back(ch);
        }
    }
    return result;
}
struct LoadAvg
{
    double oneMinute;
    double fiveMinutes;
    double fifteenMinutes;
};

string formatLoadAvg(const string *str)
{
    LoadAvg loadAvg;
    istringstream iss(*str);
    iss >> loadAvg.oneMinute >> loadAvg.fiveMinutes >> loadAvg.fifteenMinutes;
    stringstream ss;
    ss << loadAvg.oneMinute << "|"
       << loadAvg.fiveMinutes << "|"
       << loadAvg.fifteenMinutes;
    return ss.str();
}
