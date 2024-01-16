#include "nt.hpp"
#include "nt_def.hpp"

#include <vector>
#include <stdexcept>
#include <Windows.h>
#include <winternl.h>

std::size_t nt::find_process_id(std::wstring_view process_name)
{
    std::uint32_t size = 0;
    NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, reinterpret_cast<ULONG*>(&size));

    if (!size)
    {
        throw std::runtime_error("failed to query SystemProcessInformation buffer size");
    }

    std::uint32_t buffer_size = size;
    std::vector<std::uint8_t> system_information_buffer(buffer_size);

    SYSTEM_PROCESS_INFORMATION* system_information = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(system_information_buffer.data());

    if (!system_information)
    {
        throw std::runtime_error("failed to alloc SystemProcessInformation buffer");
    }

    if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessInformation, system_information, buffer_size, reinterpret_cast<ULONG*>(&size))))
    {
        throw std::runtime_error("failed to query SystemProcessInformation");
    }

    std::size_t process_id = 0;

    while (system_information->NextEntryOffset)
    {
        std::wstring_view image_name_view(system_information->ImageName.Buffer, system_information->ImageName.Length / sizeof(wchar_t));

        if (image_name_view.find(process_name) != std::wstring::npos)
        {
            process_id = reinterpret_cast<std::size_t>(system_information->UniqueProcessId);
            break;
        }

        system_information = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<std::uint8_t*>(system_information) + system_information->NextEntryOffset);
    }

    return process_id;
}

std::uint8_t* nt::find_kernel_module(std::string_view module_name)
{
    std::uint32_t size = 0;
    NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(0x0B), nullptr, 0, reinterpret_cast<ULONG*>(&size));

    if (!size)
    {
        throw std::runtime_error("failed to query SystemModuleInformation buffer size");
    }

    std::uint32_t buffer_size = size;
    std::vector<std::uint8_t> buffer(buffer_size);

    nt_def::s_process_modules* process_modules = reinterpret_cast<nt_def::s_process_modules*>(buffer.data());

    if (!process_modules)
    {
        throw std::runtime_error("failed to alloc SystemModuleInformation buffer");
    }

    if (!NT_SUCCESS(NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(0x0B), process_modules, buffer_size, reinterpret_cast<ULONG*>(&size))))
    {
        throw std::runtime_error("failed to query SystemModuleInformation");
    }

    std::uint8_t* image_base = 0;

    for (std::uint32_t i = 0; i < process_modules->numer_of_modules; i++)
    {
        nt_def::s_process_module_information& module = process_modules->modules[i];

        std::string_view current_module_name_view = reinterpret_cast<char*>(module.full_path_name + module.offset);

        if (current_module_name_view.find(module_name) != std::string::npos)
        {
            image_base = reinterpret_cast<std::uint8_t*>(module.image_base);
            break;
        }
    }

    return image_base;
}

std::uint8_t* nt::find_module(std::wstring_view module_name)
{
#ifdef _M_X64
    PEB* peb = reinterpret_cast<PEB*>(__readgsqword(0x60));
#else
    PEB* peb = reinterpret_cast<PEB*>(__readfsdword(0x30));
#endif

    if (!peb)
    {
        throw std::runtime_error("failed to find process environment block");
    }

    PEB_LDR_DATA* ldr = peb->Ldr;

    if (!ldr)
    {
        throw std::runtime_error("failed to ldr data");
    }

    std::uint8_t* image_base = 0;

    for (LIST_ENTRY* entry = (&ldr->InMemoryOrderModuleList)->Flink; entry != &ldr->InMemoryOrderModuleList; entry = entry->Flink)
    {
        LDR_DATA_TABLE_ENTRY* ldr_entry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        std::wstring_view dll_name_view(ldr_entry->FullDllName.Buffer, ldr_entry->FullDllName.Length / sizeof(wchar_t));

        if (dll_name_view.find(module_name) != std::wstring::npos)
        {
            image_base = static_cast<std::uint8_t*>(ldr_entry->DllBase);
            break;
        }
    }

    return image_base;
}
