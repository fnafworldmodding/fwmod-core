#ifndef COMMON_H
#define COMMON_H
#pragma once
#ifdef FWMODEXPORTS
#define DLLCALL __declspec(dllexport)
#else
#define DLLCALL __declspec(dllimport)
#endif


#endif // COMMON_H