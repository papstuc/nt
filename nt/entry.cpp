#include <cstdint>
#include <iostream>

#include "nt.hpp"

std::int32_t __cdecl main(std::int32_t argc, char** argv)
{
	std::size_t explorer_pid = 0;
	std::uint8_t* ntoskrnl_base = nullptr;
	std::uint8_t* ntdll_base = nullptr;

	try
	{
		explorer_pid = nt::find_process_id(L"explorer.exe");
		ntoskrnl_base = nt::find_kernel_module("ntoskrnl.exe");
		ntdll_base = nt::find_module(L"ntdll");
	}
	catch (std::exception& exception)
	{
		std::printf("%s\n", exception.what());
		return 0;
	}

	std::printf("explorer.exe pid -> %zu\n", explorer_pid);
	std::printf("ntoskrnl.exe kernel base -> %p\n", ntoskrnl_base);
	std::printf("ntdll base -> %p\n", ntdll_base);

	return 0;
}