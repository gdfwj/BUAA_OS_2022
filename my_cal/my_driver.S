#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
LEAF(_my_getchar)
	lui t0, 0xB000
	lb t1, 0(t0)
	sb t1, 0(t0)
	jr ra
END(_my_getchar)
LEAF(_my_putchar)
	loop:
		beq zero, a0, loop
	nop
	lui t0, 0xB000
	sb a0, 0(t0)
	jr ra
END(_my_putchar)
LEAF(_my_exit)
	lui t0, 0xB000
	addi t0, t0, 0x10
	sb t0, 0(t0)
	jr ra
END(_my_exit)
