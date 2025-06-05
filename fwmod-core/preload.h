#ifndef PRELOAD_H
#define PRELOAD_H
#pragma once
#include <atomic>

#define static_short(value) static_cast<short>(value)
extern std::atomic<bool> PreloadStateReady;

void StartPreloadProcess();

#endif // PRELOAD_H