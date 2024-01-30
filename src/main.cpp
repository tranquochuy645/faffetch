#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <sstream>
#include <sys/utsname.h>
#include "helpers.hpp"
#include "logo.hpp"

#define TERMINAL_HEIGHT 32
#define KEY_COL_WIDTH 36
#define RIGHT_COL_WIDTH 60
#define BOLD_LOGO_COLOR "\e[1m\e[38;5;3m"
#define RESET "\e[0m"
#define RESET_TAB_BOLD "\e[0m\e[1m\t"
using namespace std;

static const char *work_dir;

// Right column's height should equal to the logo's height for better view
static stringstream right_col[LOGO_HEIGHT];

static stringstream systemctl;

static void getSystemctlInfo()
{
    int sysctlStatus = system("pidof systemd > /dev/null");
    if (sysctlStatus != 0)
    {
        systemctl << "\e[38;5;1m!!!! Systemd is not running!";
        return;
    }
    istringstream sysctlFailedOutput(
        static_cast<ostringstream &&>(
            ostringstream{} << system("systemctl --failed | grep \"loaded units listed\" | head -c 1"))
            .str());
    int sysctlNumFailed;
    sysctlFailedOutput >> sysctlNumFailed;
    if (sysctlNumFailed > 0)
    {
        systemctl << "\e[38;5;3m!!!! " << to_string(sysctlNumFailed) << "service";
        sysctlNumFailed == 1 && systemctl << "s ";
        systemctl << "failed!";
        return;
    }
    systemctl << "\e[38;5;2m!!!! All services OK!";
}

static void getLoadAvg(const int row_i)
{
    ifstream loadavg_file("/proc/loadavg");
    string line;
    getline(loadavg_file, line); // Get the first line then close
    loadavg_file.close();
    right_col[row_i] << setw(KEY_COL_WIDTH) << "Load average:";
    right_col[row_i] << RESET_TAB_BOLD
                     << formatLoadAvg(&line);
    ifstream cpus_file("/sys/devices/system/cpu/online");
    getline(cpus_file,line);
    right_col[row_i]<<" (CPUs "<<line<<")";
    cpus_file.close();
}
static void getUptime(const int row_i)
{
    ifstream uptime_file("/proc/uptime");
    long uptime_seconds = 0;
    if (uptime_file.is_open())
    {
        uptime_file >> uptime_seconds;
    }
    uptime_file.close();
    right_col[row_i] << setw(KEY_COL_WIDTH) << "Uptime:";
    right_col[row_i] << RESET_TAB_BOLD
                     << setfill('0')
                     << setw(2) << uptime_seconds / 3600 << "h:"
                     << setfill('0') << setw(2) << (uptime_seconds % 3600) / 60 << "m:"
                     << setfill('0') << setw(2) << uptime_seconds % 60 << "s";
}
static void getKernel(const int row_i)
{
    struct utsname buffer;
    errno = 0;
    if (uname(&buffer) != 0)
    {
        perror("uname");
        exit(EXIT_FAILURE);
    }
    right_col[row_i] << setw(KEY_COL_WIDTH) << "Kernel:";
    right_col[row_i] << RESET_TAB_BOLD << buffer.release;
}

static void getMemInfo(const int row_i, const int max_rows)
{
    ifstream meminfo_file("/proc/meminfo");
    string line;
    int i = 0;
    while (i < max_rows && getline(meminfo_file, line))
    {
        if (line.find("MemF") != string::npos ||
            line.find("MemT") != string::npos ||
            line.find("SwapF") != string::npos ||
            line.find("SwapT") != string::npos)
        {
            int pos = line.find(":");
            right_col[row_i + i] << setw(KEY_COL_WIDTH) << line.substr(0, ++pos);
            right_col[row_i + i] << RESET_TAB_BOLD << removeWhitespaces(line.substr(++pos));
            i++;
        }
    }
    meminfo_file.close();
}

static void readNetInfoCache(const int row_i, const int max_rows)
{
    stringstream real_path;
    real_path << work_dir << "/cache.csv";
    ifstream netinfo_file(real_path.str());
    string line;
    int i = 0;
    while (i < max_rows && getline(netinfo_file, line))
    {
        int pos = line.find(",");
        right_col[row_i + i] << setw(KEY_COL_WIDTH - 1) << line.substr(0, pos) << ":";
        right_col[row_i + i] << RESET_TAB_BOLD << line.substr(++pos);
        i++;
    }
    netinfo_file.close();
}
static void render()
{
    for (int i = 1; i < (TERMINAL_HEIGHT - LOGO_HEIGHT) / 2; i++)
    {
        cout << endl;
    }
    for (int i = 0, j = 0; i < LOGO_HEIGHT && left_col[i] != nullptr; i++)
    {
        cout << BOLD_LOGO_COLOR;
        cout << setw(RIGHT_COL_WIDTH) << left_col[i];
        cout << RESET;

        while (j < LOGO_HEIGHT && right_col[j].str().empty())
        {
            j++;
        }

        if (j < LOGO_HEIGHT)
        {
            cout << right_col[j].str();
            j++;
        }
        cout << endl;
    }

    // This final line wil be bold because the last right column doesn't reset the boldness
    cout << endl;
    cout << setw(RIGHT_COL_WIDTH) << systemctl.str();
    cout << RESET;
    for (int i = 0; i < (TERMINAL_HEIGHT - LOGO_HEIGHT) / 2; i++)
    {
        cout << endl;
    }
}
// // Declare an array of threads
static thread threads[6];
// change the length of the array exactly to the number of threads you use, or it will crash

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        cout << "wrong number of arguments";
        return 1;
    }
    else if (argc == 2)
    {
        work_dir = argv[1];
    }
    else
    {
        work_dir = ".";
    }

    // As long as no threads overlap rows with each other, this will work

    // threads[5] = thread(getPublicIp, 11);
    threads[0] = thread(getSystemctlInfo);
    threads[5] = thread(getLoadAvg, 2);
    // This thread will write to many rows, mapped to by start row index and max_rows (count)
    threads[4] = thread(readNetInfoCache, 7, 9);

    // This thread will write to many rows, mapped to by start row index and max_rows (count)
    threads[3] = thread(getMemInfo, 3, 4);

    threads[2] = thread(getUptime, 1);
    threads[1] = thread(getKernel, 0);

    // Block until all threads finish
    for (thread &t : threads)
    {
        t.join();
    }

    // render the final output
    render();
    return 0;
}
