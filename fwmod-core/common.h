#ifndef COMMON_H
#define COMMON_H
#pragma once
#ifdef FWMODEXPORTS
#define DLLCALL __declspec(dllexport)
#else
#define DLLCALL __declspec(dllimport)
#endif
constexpr auto FWMOD_CORE_VERSION_STRING = "1.0.0a";
#define FWMOD_CORE_VERSION_MAJOR 1f

#endif // COMMON_H