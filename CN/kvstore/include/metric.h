#pragma once

struct Metrics {
    long requests = 0;
    long connections = 0;
};

extern Metrics metrics;