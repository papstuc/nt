#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace nt
{
	// kernel handles process ids as a void* which is 4 bytes on x86 and 8 bytes on x64
	std::size_t find_process_id(std::wstring_view process_name);

	std::uint8_t* find_kernel_module(std::string_view module_name);

	std::uint8_t* find_module(std::wstring_view module_name);
}