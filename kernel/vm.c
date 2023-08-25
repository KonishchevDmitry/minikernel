// See https://wiki.osdev.org/Paging for details

// FIXME(konishchev): HERE

// Page Directory Entry (PDE) structure
typedef struct {
    unsigned int present : 1;     // Page present in memory
    unsigned int rw : 1;          // Read-write access
    unsigned int user : 1;        // User/supervisor mode
    unsigned int accessed : 1;    // Page accessed since last refresh
    unsigned int reserved : 1;    // Reserved, set to 0
    unsigned int large : 1;       // Page size (0 = 4KB, 1 = 4MB)
    unsigned int global : 1;      // Global page (ignored in 32-bit mode)
    unsigned int available : 3;   // Available for software use
    unsigned int page_frame : 20; // Page frame address (physical address >> 12)
} PageDirectoryEntry;

// Page Table Entry (PTE) structure
typedef struct {
    unsigned int present : 1;     // Page present in memory
    unsigned int rw : 1;          // Read-write access
    unsigned int user : 1;        // User/supervisor mode
    unsigned int accessed : 1;    // Page accessed since last refresh
    unsigned int dirty : 1;       // Page has been written to since last refresh
    unsigned int reserved : 1;    // Reserved, set to 0
    unsigned int global : 1;      // Global page (ignored in 32-bit mode)
    unsigned int available : 3;   // Available for software use
    unsigned int page_frame : 20; // Page frame address (physical address >> 12)
} PageTableEntry;