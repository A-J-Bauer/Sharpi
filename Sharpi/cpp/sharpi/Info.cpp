#include "Info.h"

using namespace std;

void Info::GetMemory(unsigned long& total, unsigned long& vmsize, unsigned long& rsize, unsigned long& rshrd, unsigned long& rprvt)
{
    unsigned long pageSize = sysconf(_SC_PAGE_SIZE);
    unsigned long numPages = sysconf(_SC_PHYS_PAGES);
    
    total = pageSize * numPages;

    unsigned long size = 0, resident = 0, shared = 0;
    ifstream buffer("/proc/self/statm");
    buffer >> size >> resident >> shared;
    buffer.close();

    vmsize = size * pageSize;
    rsize = resident * pageSize;
    rshrd = shared * pageSize;
    rprvt = rsize - rshrd;

}