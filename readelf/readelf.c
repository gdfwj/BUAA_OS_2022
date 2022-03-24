/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
                ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
                ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
                ehdr->e_ident[EI_MAG3] == ELFMAG3) {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

        int Nr;

        Elf32_Phdr *shdr = NULL;

        u_char *ptr_sh_table = NULL;
        Elf32_Half sh_entry_count;
        Elf32_Half sh_entry_size;


        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary)) {
                printf("not a standard elf format\n");
                return 0;
        }

        // get section table addr, section header number and section header size.
		ptr_sh_table = binary+ehdr->e_phoff;
		sh_entry_count = ehdr->e_phnum;
		sh_entry_size = ehdr->e_phentsize;
		//shdr=(Elf32_Shdr*)ptr_sh_table;
        // for each section header, output section number and section addr.
		int lastend=0;
		int flag=0;
		for(Nr = 0; Nr < sh_entry_count; Nr++){
			shdr = (Elf32_Phdr*)(ptr_sh_table+Nr*sh_entry_size);
			if((shdr->p_vaddr<lastend)){
				printf("Conflict at page va : 0x%x\n", lastend & 0x1000);
				return 0;
			}
			else if((shdr->p_vaddr<((lastend&0x100)+0x100))){
				printf("Overlay at page va : 0x%x\n", lastend & 0x100);
				return 0;
			}
			lastend = shdr-> p_memsz + shdr->p_vaddr;
		}
			for (Nr = 0; Nr < sh_entry_count; Nr++) {
				shdr = (Elf32_Phdr*)(ptr_sh_table+Nr*sh_entry_size);
				printf("%d:0x%x,0x%x\n", Nr, shdr->p_filesz, shdr-> p_memsz);
			//shdr =(Elf32_Shdr*)((u_char*) shdr + sh_entry_size);
			} 
        // hint: section number starts at 0.


        return 0;
}

