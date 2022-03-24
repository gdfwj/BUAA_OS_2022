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
		Elf32_Half start[10], end[10];
		for(Nr = 0; Nr < sh_entry_count; Nr++){
			shdr = (Elf32_Phdr*)(ptr_sh_table+Nr*sh_entry_size);
			start[Nr] = shdr->p_vaddr;
			end[Nr] = shdr->p_memsz;
		}
		Elf32_Half temp;
		int i,j;
		for(i=0;i<Nr;i++){
			for(j=0;j<Nr-i-2;j++){
				if(start[j]>start[j+1]){
					temp = start[j];
					start[j] = start[j+1];
					start[j+1] = temp;
					temp = end[j];
					end[j] = end[j+1];
					end[j+1] = temp;
				}
			}
		}
		temp = end[0];
		for(i=1;i<Nr;i++){
			if(start[i]<temp){
				printf("(Conflict at page va : 0x%x\n", temp & 0x1000);
				return 0;
			}
			else if(start[i]<((temp&0x1000)+0x1000)){
				printf("Overlay at page va : 0x%x\n", temp & 0x1000);
				return 0;
			}
		}
			for (Nr = 0; Nr < sh_entry_count; Nr++) {
				shdr = (Elf32_Phdr*)(ptr_sh_table+Nr*sh_entry_size);
				printf("%d:0x%x,0x%x\n", Nr, shdr->p_filesz, shdr-> p_memsz);
			//shdr =(Elf32_Shdr*)((u_char*) shdr + sh_entry_size);
			} 
        // hint: section number starts at 0.


        return 0;
}

