/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes./
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	u_int dev = 0x13000000;
	u_char read_mod = 0;
	int status;
	while (offset_begin + offset < offset_end) {
		u_int cur_offset = offset_begin + offset;
		if (syscall_write_dev((u_int)&diskno, dev+0x10, 4)<0) {
            user_panic("IDE_read_error when select by id!\n");
        }

        if (syscall_write_dev((u_int)&cur_offset, dev, 4)<0) {
            user_panic("IDE_read_error when setting offset!\n");
        }

        if (syscall_write_dev((u_int)&read_mod, dev+0x20, 1)<0) {
            user_panic("IDE_read_error when setting read_mod!\n");
        }
		status = 0;
        if (syscall_read_dev((u_int)&status, dev+0x30, 1)<0) {
            user_panic("IDE_read_error when getting IDE status!\n");
        }

        if (status==0) {
            return -1;
        }
        if (syscall_read_dev((u_int)(dst+offset), dev+0x4000, 0x200)<0) {
            user_panic("IDE_read_error when reading data!\n");
        }
        offset += 0x200;
		// Your code here
		// error occurred, then panic.
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	 int offset_begin = secno*0x200;
	 int offset_end = offset_begin + nsecs*0x200;
	 int offset = 0;
	 u_int dev = 0x13000000;
     u_char status = 0;
     u_char write_mod = 1;
	// DO NOT DELETE WRITEF !!!
	 writef("diskno: %d\n", diskno);
	 while ( offset_begin + offset < offset_end ) {
		u_int cur_offset = offset_begin + offset;
        if (syscall_write_dev((u_int)&diskno, dev+0x10, 4)<0) {
            user_panic("IDE_write_error when select by id!\n");
        }
        if (syscall_write_dev((u_int)&cur_offset, dev, 4)<0) {
            user_panic("IDE_write_error when setting offset!\n");
        }
        if (syscall_write_dev((u_int)(src+offset), dev+0x4000, 0x200)<0) {
            user_panic("IDE_write_error when writing data!\n");
        }
        if (syscall_write_dev((u_int)&write_mod, dev+0x20, 1)<0) {
            user_panic("IDE_write_error when setting write_mod!\n");
        }
        status = 0;
        if (syscall_read_dev((u_int)&status, dev+0x30, 1)<0) {
            user_panic("IDE_write_error when getting IDE status!\n");
        }
        if (status==0) {
            user_panic("IDE_write_error for IDE failed!\n");
        }

        offset += 0x200;
		// copy data from source array to disk buffer.

		// if error occur, then panic.
	 }
}
int raid4_valid(u_int diskno) {
	u_int dev = 0x13000000;
	u_char read_mod = 0;
	int status;
	u_int cur_offset = 0;
		if (syscall_write_dev((u_int)&diskno, dev+0x10, 4)<0) {
            user_panic("IDE_read_error when select by id!\n");
        }

        if (syscall_write_dev((u_int)&cur_offset, dev, 4)<0) {
            user_panic("IDE_read_error when setting offset!\n");
        }

        if (syscall_write_dev((u_int)&read_mod, dev+0x20, 1)<0) {
            user_panic("IDE_read_error when setting read_mod!\n");
        }
		status = 0;
        if (syscall_read_dev((u_int)&status, dev+0x30, 1)<0) {
            user_panic("IDE_read_error when getting IDE status!\n");
        }
        if (status==0) {
            return 0;
        }
	return 1;
}
int raid4_write(u_int blockno, void *src) {
	int invalid[6]={0};
	int invalidcount=0;
	u_int j;
	u_int offset=0;
	u_int i;
	char *srcc = src;
	char checksum[1000];
	user_bzero(checksum, 0x200);
	for(i=1;i<=5;i++) {
		if(raid4_valid(i)==0) {
			invalid[i]=1;
			invalidcount++;
		}
	}
	for(i=1;i<=4;i++) {
		if(invalid[i]==0) {
			ide_write(i, blockno*2, src+offset, 1);
		}
			for(j=0;j<0x200;j++) {
				checksum[j] = checksum[j] ^ srcc[j+offset];
			}
		offset+=0x200;
	}
	if(invalid[5]==0) ide_write(5, blockno*2, checksum, 1);
	user_bzero(checksum, 0x200);
	for(i=1;i<=4;i++) {
		if(invalid[i]==0) {
			ide_write(i, blockno*2+1, src+offset, 1);
		}
			for(j=0;j<0x200;j++) {
				checksum[j] = checksum[j] ^ srcc[j+offset];
			}
		offset+=0x200;
	}
	if(invalid[5]==0) ide_write(5, blockno*2+1, checksum, 1);
	return invalidcount;
}
int raid4_read(u_int blockno, void *src) {
	int invalid[6]={0};
	int invalidcount=0;
	u_int j;
	u_int offset=0;
	u_int i;
	int falt=0;
	int flag=0;
	char *srcc = src;
	char checksum[1000]={0};
	char r5[1000]={0};
	user_bzero(checksum, 0x200);
	for(i=1;i<=5;i++) {
		if(raid4_valid(i)==0) {
			invalid[i]=1;
			falt=i;
			invalidcount++;
		}
	}
	if(invalidcount>1) return invalidcount;
	else if(invalidcount==0) {
		for(i=1;i<=4;i++) {
			ide_read(i, blockno*2, src+offset, 1);
			for(j=0;j<0x200;j++) {
				checksum[j] = checksum[j] ^ srcc[j+offset];
			}
			offset+=0x200;
		}
		ide_read(5, blockno*2, r5, 1);
		for(j=0;j<0x200;j++) {
			if(r5[j]!=checksum[j]) {
				flag=1;
			}
		}
		user_bzero(checksum, 0x200);
		for(i=1;i<=4;i++) {
			ide_read(i, blockno*2+1, src+offset, 1);
			for(j=0;j<0x200;j++) {
				checksum[j] = checksum[j] ^ srcc[j+offset];
			}
			offset+=0x200;
		}
		ide_read(5, blockno*2+1, r5, 1);
		for(j=0;j<0x200;j++) {
			if(r5[j]!=checksum[j]) {
				flag=1;
			}
		}
		if(flag==1) return -1;
		return 0;
	}
	else if(falt!=5){
		for(i=1;i<=4;i++) {
			if(invalid[i]==0) {
				ide_read(i, blockno*2, src+offset, 1);
				for(j=0;j<0x200;j++) {
					checksum[j] = checksum[j] ^ srcc[j+offset];
				}
			}
			offset+=0x200;
		}
		ide_read(5, blockno*2, r5, 1);
		for(j=0;j<0x200;j++) {
			checksum[j] ^= r5[j];
		}
		user_bcopy(checksum, src+(falt-1)*0x200, 0x200);
		user_bzero(checksum, 0x200);
		for(i=1;i<=4;i++) {
			if(invalid[i]==0) {
				ide_read(i, blockno*2+1, src+offset, 1);
				for(j=0;j<0x200;j++) {
					checksum[j] ^= srcc[j+offset];
				}
			}
			offset+=0x200;
		}
		ide_read(5, blockno*2+1, r5, 1);
		for(j=0;j<0x200;j++) {
			checksum[j] ^=r5[j];
		}
		user_bcopy(checksum, src+(falt-1)*0x200+0x800, 0x200);
		return 1;
	} else {
		for(i=1;i<=4;i++) {
			ide_read(i, blockno*2, src+offset, 1);
			offset+=0x200;
		}
		for(i=1;i<=4;i++) {
			ide_read(i, blockno*2+1, src+offset, 1);
			offset+=0x200;
		}
		return 1;
	}
	
}
