#pragma once

#include <iostream>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <print>
#include <stdio.h>
#include <sal.h>
#include <conio.h>
#include <string.h>
#include <functional>
#include <system_error>
template<typename T>
void run(std::function<T()> fn);

DECLSPEC_NORETURN void fail(DWORD error = GetLastError(), const std::string msg = "");
