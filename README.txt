nt - game hacking focused winapi functions wrapped in modern c++ 
-----------------------------------------------------------------

this small library tries to combine the old C winapi functions into a portable and modern c++ library which can then be used in any project of your choice.
it uses raii for dynamic memory allocations, throws exceptions on any error and expects views of strings to be passed in as parameters to avoid overhead.

-----------------------------------------------------------------

current following functions are implemented:

* nt::find_process_id: finds a process id without opening any handles, by querying SystemProcessInformation
* nt::find_kernel_module: finds a kernel module's base address from usermode, by querying the undocumented information class SystemModuleInformation
* nt::find_module: finds the base of a module inside process context using the process environment block

-----------------------------------------------------------------

contributing: any help is greatly appreciated! simply open a pr, describe what your addition does, why it should be added and wait for replies and a possible merge.