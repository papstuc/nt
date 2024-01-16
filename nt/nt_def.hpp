#pragma once

#include <cstdint>
#include <cstddef>

namespace nt_def
{
    struct s_process_module_information
    {
        std::size_t section;
        std::size_t mapped_base;
        std::size_t image_base;
        std::uint32_t image_size;
        std::uint32_t flags;
        std::uint16_t load_order_index;
        std::uint16_t init_order_index;
        std::uint16_t load_count;
        std::uint16_t offset;
        std::uint8_t full_path_name[256];
    };

    struct s_process_modules
    {
        std::size_t numer_of_modules;
        s_process_module_information modules[1];
    };
}