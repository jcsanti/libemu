/* @header@ */
#include <stdint.h>
#include <errno.h>

#define INSTR_CALC(bits, a, cpu) \
UINT(bits) operand_a = a; \
UINT(bits) operation_result = ~operand_a; \
a = operation_result; 

#include "emu/emu.h"
#include "emu/emu_cpu.h"
#include "emu/emu_cpu_data.h"
#include "emu/emu_cpu_functions.h"
#include "emu/emu_cpu_stack.h"
#include "emu/emu_memory.h"

/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 497*/

#define INSTR_CALC_AND_SET_FLAGS(bits, cpu, a)	\
INSTR_CALC(bits, a, cpu)

int32_t instr_group_3_f6_not(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	if (i->modrm.mod != 3)
	{
		/* F6 /2  
		 * Reverse each bit of r/m8
		 * NOT r/m8    
		 */
		uint8_t m8;
		MEM_BYTE_READ(c, i->modrm.ea, &m8);
		INSTR_CALC_AND_SET_FLAGS(8,
								 c,
								 m8)
		MEM_BYTE_WRITE(c, i->modrm.ea, m8);

	}
	else
	{
		/* F6 /2  
		 * Reverse each bit of r/m8
		 * NOT r/m8    
		 */
		INSTR_CALC_AND_SET_FLAGS(8,c,*c->reg8[i->modrm.rm]);
	}

	return 0;
}

int32_t instr_group_3_f7_not(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	if ( i->modrm.mod != 3 )
	{
		if ( i->prefixes & PREFIX_OPSIZE )
		{
			/* F7 /2  
			 * Reverse each bit of r/m16
			 * NOT r/m16   
			 */
			uint16_t m16;
			MEM_WORD_READ(c, i->modrm.ea, &m16);
			INSTR_CALC_AND_SET_FLAGS(16,
									 c,
									 m16)
			MEM_WORD_WRITE(c, i->modrm.ea, m16);

		}
		else
		{
			/* F7 /2  
			 * Reverse each bit of r/m32
			 * NOT r/m32   
			 */
			uint32_t m32;
			MEM_DWORD_READ(c, i->modrm.ea, &m32);
			INSTR_CALC_AND_SET_FLAGS(32,
									 c,
									 m32)
			MEM_DWORD_WRITE(c, i->modrm.ea, m32);

		}
	}
	else
	{
		if ( i->prefixes & PREFIX_OPSIZE )
		{
			/* F7 /2  
			 * Reverse each bit of r/m16
			 * NOT r/m16   
			 */   
			INSTR_CALC_AND_SET_FLAGS(16,
									 c,
									 *c->reg16[i->modrm.rm])

		}
		else
		{
			/* F7 /2  
			 * Reverse each bit of r/m32
			 * NOT r/m32   
			 */
			INSTR_CALC_AND_SET_FLAGS(32,
									 c,
									 c->reg[i->modrm.rm])

		}

	}
	return 0;
}
