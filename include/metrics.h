#ifndef METRICS_H
#define METRICS_H

#include <stdbool.h>

typedef struct {
    unsigned long mem_total_kb;
    unsigned long mem_free_kb;
    unsigned long mem_available_kb;
} MemoryMetrics;

bool fetch_memory_metrics(MemoryMetrics *metrics);

#endif // METRICS_H
