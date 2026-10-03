#include "../include/metrics.h"
#include <stdio.h>
#include <stdbool.h>

bool fetch_memory_metrics(MemoryMetrics *metrics) {
    if (!metrics) return false;

    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return false;

    char line[256];
    int parsed_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %lu kB", &metrics->mem_total_kb) == 1) {
            parsed_count++;
        } else if (sscanf(line, "MemFree: %lu kB", &metrics->mem_free_kb) == 1) {
            parsed_count++;
        } else if (sscanf(line, "MemAvailable: %lu kB", &metrics->mem_available_kb) == 1) {
            parsed_count++;
        }

        if (parsed_count >= 3) break;
    }

    fclose(fp);
    return parsed_count >= 3;
}
