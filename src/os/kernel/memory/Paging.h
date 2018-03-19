/* Some macros and constants used for paging
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 */

#ifndef __PAGING_H__
#define __PAGING_H__

 //Page Directory only Flags
#define PAGE_SIZE_KiB			        0x000
#define PAGE_SIZE_MiB			        0x080
// this bit will not be used (OS could use it for reserved pages ??)
#define PAGE_SIZE_IGNORED               0x100 

//Page Table only Flags
#define PAGE_NOT_DIRTY		            0x000
#define PAGE_DIRTY				        0x040
#define PAGE_NOT_GLOBAL		            0x000
// prevents the TLB from updating the address in its cache if CR3 is reset (Bit in CR4 must be set)
#define PAGE_GLOBAL				        0x100 


//Common Flags
#define PAGE_NOT_PRESENT 	            0x000
#define PAGE_PRESENT 			        0x001
#define PAGE_READ_ONLY			        0x000
#define PAGE_READ_WRITE			        0x002
#define PAGE_ACCESS_SUPERVISOR	        0x000
#define PAGE_ACCESS_ALL			        0x004
#define PAGE_WRITE_BACK			        0x000
#define PAGE_WRITE_THROUGH		        0x008
#define PAGE_CACHING			        0x000
#define PAGE_NO_CACHING			        0x010
#define PAGE_NOT_ACCESSED		        0x000
#define PAGE_ACCESSED			        0x020
#define PAGE_PROTECTED                  0x200

#define GET_PD_IDX(x)                   (x >> 22)
#define GET_PT_IDX(x)                   ((x >> 12) & 0x3FF)
#define GET_OFFSET(x)                   (x & 0xFFF)
#define GET_FLAGS(x)                    (x & 0xFFF)

// pagesize
#define PAGESIZE 0x1000

#endif