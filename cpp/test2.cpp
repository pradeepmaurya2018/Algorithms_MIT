#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/statvfs.h>

void print_os_kernel() {
    struct utsname uts;
    uname(&uts);

    printf("OS / Kernel\n");
    printf("-----------\n");
    printf("System   : %s\n", uts.sysname);
    printf("Node     : %s\n", uts.nodename);
    printf("Release  : %s\n", uts.release);
    printf("Version  : %s\n", uts.version);
    printf("Machine  : %s\n\n", uts.machine);
}

void print_cpu_info() {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    char line[256];
    int cores = 0;

    printf("CPU Info\n");
    printf("--------\n");

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            printf("%s", line);
            break;
        }
    }

    rewind(fp);
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processor", 9) == 0)
            cores++;
    }

    fclose(fp);
    printf("CPU Cores : %d\n\n", cores);
}

void print_memory_info() {
    struct sysinfo info;
    sysinfo(&info);

    printf("Memory (RAM)\n");
    printf("------------\n");
    printf("Total RAM     : %.2f GB\n",
           (info.totalram * info.mem_unit) / (1024.0 * 1024 * 1024));
    printf("Available RAM : %.2f GB\n",
           (info.freeram * info.mem_unit) / (1024.0 * 1024 * 1024));
    printf("Uptime        : %ld hours\n\n",
           info.uptime / 3600);
}

void print_disk_info() {
    struct statvfs stat;

    statvfs("/", &stat);

    unsigned long total =
        stat.f_blocks * stat.f_frsize;
    unsigned long free =
        stat.f_bfree * stat.f_frsize;

    printf("Disk (/)\n");
    printf("---------\n");
    printf("Total Disk : %.2f GB\n",
           total / (1024.0 * 1024 * 1024));
    printf("Free Disk  : %.2f GB\n\n",
           free / (1024.0 * 1024 * 1024));
}

int main() {
    printf("\n===== LINUX SYSTEM INFORMATION =====\n\n");
    print_os_kernel();
    print_cpu_info();
    print_memory_info();
    print_disk_info();
    printf("===================================\n");
    return 0;
}
