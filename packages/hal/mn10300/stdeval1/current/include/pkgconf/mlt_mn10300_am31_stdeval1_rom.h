// eCos memory layout - Tue Jul 25 19:36:26 2000

// This is a generated file - do not edit

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_rom (0x40000000)
#define CYGMEM_REGION_rom_SIZE (0x80000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
#define CYGMEM_REGION_ram (0x48000000)
#define CYGMEM_REGION_ram_SIZE (0x200000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
extern char CYG_LABEL_NAME (__heap1) [];
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (0x48200000 - (size_t) CYG_LABEL_NAME (__heap1))
