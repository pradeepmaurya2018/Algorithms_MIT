#include <iostream>
#include <stdexcept>
using namespace std;

/* ------------------ Constants ------------------ */

const int PAGE_SIZE = 4096;
const int MAX_PAGES = 16;
const int MAX_FRAMES = 8;

/* ------------------ Structures ------------------ */

// One virtual page
struct Page {
    bool present = false;
    bool dirty = false;
    int frame = -1;
};

// Page table (per process)
struct PageTable {
    Page pages[MAX_PAGES];
};

// Physical memory frame
struct Frame {
    bool free = true;
    int ownerPid = -1;
};

// Process
struct Process {
    int pid;
    PageTable pageTable;
};

/* ------------------ Global Physical Memory ------------------ */

Frame physicalMemory[MAX_FRAMES];

/* ------------------ Helper Functions ------------------ */

int findFreeFrame() {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (physicalMemory[i].free) {
            return i;
        }
    }
    return -1;
}

void handlePageFault(Process& p, int pageNumber) {
    cout << "[OS] Page fault in process " << p.pid
         << " for page " << pageNumber << endl;

    int frame = findFreeFrame();
    if (frame == -1) {
        throw runtime_error("Out of physical memory!");
    }

    physicalMemory[frame].free = false;
    physicalMemory[frame].ownerPid = p.pid;

    p.pageTable.pages[pageNumber].present = true;
    p.pageTable.pages[pageNumber].frame = frame;

    cout << "[OS] Loaded page " << pageNumber
         << " into frame " << frame << endl;
}

int translateAddress(Process& p, int virtualAddress) {
    int pageNumber = virtualAddress / PAGE_SIZE;
    int offset = virtualAddress % PAGE_SIZE;

    Page& page = p.pageTable.pages[pageNumber];

    if (!page.present) {
        handlePageFault(p, pageNumber);
    }

    return page.frame * PAGE_SIZE + offset;
}

/* ------------------ Simulation ------------------ */

int main() {
    cout << "=== Program start ===\n";

    // STEP 1: Create process
    Process p;
    p.pid = 42;

    cout << "[OS] Created process with PID " << p.pid << endl;

    // STEP 2: Instruction fetch (code page)
    int codeAddress = 0x0000;
    cout << "\n[CPU] Fetch instruction at virtual address "
         << codeAddress << endl;

    int phys = translateAddress(p, codeAddress);
    cout << "[MMU] Mapped to physical address " << phys << endl;

    // STEP 3: Stack write (int x = 10)
    int stackAddress = 0x7000;
    cout << "\n[CPU] Write x = 10 at virtual address "
         << stackAddress << endl;

    phys = translateAddress(p, stackAddress);
    cout << "[MMU] Write to physical address " << phys << endl;

    p.pageTable.pages[stackAddress / PAGE_SIZE].dirty = true;

    // STEP 4: Read x (x = x + 5)
    cout << "\n[CPU] Read x from virtual address "
         << stackAddress << endl;

    phys = translateAddress(p, stackAddress);
    cout << "[MMU] Read from physical address " << phys << endl;

    cout << "[CPU] x = 10, x = x + 5 -> 15\n";

    // STEP 5: Program exit
    cout << "\n[OS] Process exiting, cleaning up memory\n";

    for (int i = 0; i < MAX_FRAMES; i++) {
        if (physicalMemory[i].ownerPid == p.pid) {
            physicalMemory[i].free = true;
            physicalMemory[i].ownerPid = -1;
        }
    }

    cout << "=== Program end ===\n";
    return 0;
}
