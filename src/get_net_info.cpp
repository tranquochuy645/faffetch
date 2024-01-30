#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output)
{
    size_t total_size = size * nmemb;
    output->append((char *)contents, total_size);
    return total_size;
}
static void getLocalIp()
{
    struct ifaddrs *ifAddrStruct = nullptr;
    // Get the network interfaces
    if (getifaddrs(&ifAddrStruct) != -1)
    {

        for (struct ifaddrs *ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            {
                continue; // Skip if not an IPv4 address
            }

            // Check if the interface is active
            if (!(ifa->ifa_flags & IFF_UP))
            {
                continue; // Skip if not active
            }

            // Print the IP address of the active interface
            char ipAddr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ipAddr, INET_ADDRSTRLEN);
            cout << ifa->ifa_name << "(local)," << ipAddr << endl;
        }
    }
    // Free the memory allocated by getifaddrs
    freeifaddrs(ifAddrStruct);
}

static void getPublicIp()
{
    CURL *curl;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://ipinfo.io/json");
        // We expect a non - nested json response so that we can treat it as a map
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        istringstream iss(response);
        string line;
        while (getline(iss, line))
        {
            // Assuming each line is in the format "key: value"
            size_t i, j;
            i = line.find("readme");
            if (i != string::npos)
                continue;
            i = line.find("postal");
            if (i != string::npos)
                continue;
            i = line.find("\"");
            if (i == string::npos)
                continue;
            j = line.substr(++i).find("\"");
            cout << line.substr(i, j) << "(public),";
            i = line.substr(i + j + 1).find("\"") + j + i + 1;
            j = line.substr(++i).find("\"");
            cout << line.substr(i, j) << endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}
int main()
{
    getLocalIp();
    getPublicIp();
    return 0;
}
