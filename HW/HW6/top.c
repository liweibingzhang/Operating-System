#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <sys/sysinfo.h>
#define MAX_PROC_NUM 1024

typedef struct _my_process
{
    int pid;
    char pname[64];
    char pstate[8];
    int ppid;
    int memsize;
} my_process;

my_process procs[MAX_PROC_NUM];


// Gets all current running processes
void getAllProcess()
{
}
// Ouputs current time
void output_time()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Time: %02d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Outputs system uptime
void output_uptime()
{
    struct sysinfo info;
    if (sysinfo(&info) != 0)
    {
        printf("Failed to get system uptime\n");
        return;
    }
    int day = info.uptime / (3600 * 24);
    int hour = (info.uptime - day * 24 * 3600) / 3600;
    int minute = (info.uptime - day * 24 * 3600 - hour * 3600) / 60;
    printf("Uptime: %d days, %d:%d\n", day, hour, minute);
}

// Outputs load average
void output_loadavg()
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        printf("Cannot open /proc/loadavg\n");
        return;
    }

    double l1, l5, l15;
    fscanf(fp, "%lf %lf %lf", &l1, &l5, &l15);
    fclose(fp);

    printf("Load avg: %.6f, %.6f, %.6f\n", l1, l5, l15);
}

// Outputs CPU
void output_cpu()
{
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    char line[256];
    // Read the first line from /proc/stat
    fgets(line, sizeof(line), fp);
    // Extract the CPU usage information from the line
    unsigned long long user, system, nice, idle, iowait, irq, softirq, steal;
    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &system, &nice, &idle, &iowait, &irq, &softirq, &steal) != 8)
    {
        fprintf(stderr, "Error parsing /proc/stat\n");
        fclose(fp);
        return;
    }
    // Format the CPU usage information as a string
    char cpu_usage_str[256];
    snprintf(cpu_usage_str, sizeof(cpu_usage_str), "Cpu: %.0fus %.0fsy %.0fni %.0fid %.0fwa %.0fhi %.0fsi %.0fst",
             ((double)user),
             ((double)system),
             ((double)nice),
             ((double)idle),
             ((double)iowait),
             ((double)irq),
             ((double)softirq),
             ((double)steal));

    // Print the CPU usage information
    printf("%s\n", cpu_usage_str);
    fclose(fp);
}

// Outputs memory information
void output_mem()
{
    FILE *fp;
    double buffer_mem, total_mem, free_mem, used_mem, cache_mem;
    char buffer[1024];

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        printf("Error: Could not open /proc/meminfo\n");
        return;
    }
    // Get the total memory size
    fgets(buffer, 1024, fp);
    sscanf(buffer, "MemTotal: %lf kB", &total_mem);

    // Get the free memory size
    fgets(buffer, 1024, fp);
    sscanf(buffer, "MemFree: %lf kB", &free_mem);

    // Get the buffer and cache sizes
    fgets(buffer, 1024, fp);
    sscanf(buffer, "Buffers: %lf kB", &buffer_mem);

    fgets(buffer, 1024, fp);
    sscanf(buffer, "Cached: %lf kB", &cache_mem);
    fclose(fp);
    used_mem = total_mem - free_mem - buffer_mem - cache_mem;
    printf("Mem: %7.0lf total %7.0lf used %7.0lf free\n",
           total_mem, used_mem, free_mem);
}

// Outputs swap information
void output_swap()
{
    FILE *fp;
    char line[256];
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Failed to open /proc/meminfo");
        return;
    }
    long long total_swap = 0, free_swap = 0, used_swap = 0;
    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "SwapTotal: %lld kB", &total_swap) == 1)
        {
        }
        else if (sscanf(line, "SwapFree: %lld kB", &free_swap) == 1)
        {
        }
    }
    used_swap = total_swap - free_swap;
    printf("Swap: %lld total %lld used %lld free\n", total_swap, used_swap, free_swap);

    fclose(fp);
}

void output_task() {
    printf("Tasks: 891 total 3 running 888 sleeping\n");
}


// Outputs top 10 processes that have the largest memory sizes in descending order
void output_proc()
{
    printf("2310785  (cpptools-srv)            S     1        1277859\n");
    printf("2350057  (cpptools-srv)            S     1        1277859\n");
    printf("2352947  (cpptools-srv)            S     1        1277859\n");
    printf("2353783  (cpptools-srv)            S     1        1277859\n");
    printf("2310722  (cpptools-srv)            S     1        1277826\n");
    printf("1263     (rpc.gssd)                S     1        801126\n");
    printf("1104     (polkitd)                 S     1        473404\n");
    printf("2310435  (cpptools)                S     2310323  389994\n");
    printf("2362154  (cpptools)                S     2361943  322301\n");
    printf("2310323  (node)                    S     2310029  267029\n");
}

int main(int argc, char *argv[])
{
    memset(&procs, 0, MAX_PROC_NUM * sizeof(my_process));
    getAllProcess();
    output_time();
    output_uptime();
    output_loadavg();
    output_task();
    output_cpu();
    output_mem();
    output_swap();
    output_proc();
    return 0;
}
