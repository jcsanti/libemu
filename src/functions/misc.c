/* @header@ */
#include <stdint.h>

#include "emu/emu_cpu.h"
#include "emu/emu_cpu_data.h"
#include "emu/emu_cpu_functions.h"
#include "emu/emu_memory.h"

int32_t prefix_fn(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/* dummy */
	return 0;
}

int32_t instr_cmc_f5(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/* F5 
	 * Complement CF flag
	 * CMC 
	 */
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 107*/
	CPU_FLAG_TOGGLE(c,f_cf);
	return 0;
}


int32_t instr_clc_f8(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/* F8 
	 * Clear CF flag
	 * CLC 
	 */

	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 106*/
	CPU_FLAG_UNSET(c,f_cf);
	return 0;
}

int32_t instr_stc_f9(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/* F9 
	 * Set CF flag
	 * STC 
	 */
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 702*/
	CPU_FLAG_UNSET(c,f_cf);
	return 0;
}


int32_t instr_cld_fc(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 107*/
	CPU_FLAG_UNSET(c,f_df);
	return 0;
}

int32_t instr_std_fd(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 703*/
	/* FD 
	 * Set DF flag
	 * STD 
	 */
	CPU_FLAG_SET(c,f_df);
	return 0;
}






int32_t instr_lea_8d(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 393*/

	if ( i->prefixes & PREFIX_OPSIZE )
	{
		/* 8D /r 
		 * Store effective address for m in register r16
		 * LEA r16,m 
		 */

	}
	else
	{
		/* 8D /r 
		 * Store effective address for m in register r32
		 * LEA r32,m 
		 */
		c->reg[i->modrm.opc] = i->modrm.ea;
	}
	return 0;

}


int32_t instr_cbw_98(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 104*/

	if ( i->prefixes & PREFIX_OPSIZE )
	{
		/* 98 
		 * AX <- sign-extend of AL
		 * CBW  
		 */
		*c->reg16[ax] = (int8_t)*c->reg8[al];
	}
	else
	{
    	/* 98 
		 * EAX <- sign-extend of AX
		 * CWDE 
		 */
		c->reg[eax] = (int16_t)*c->reg16[ax];
	}
	return 0;
}


int32_t instr_cwd_99(struct emu_cpu *c, struct emu_cpu_instruction *i)
{
	/*Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference (24319102.PDF) page 181*/

	if ( i->prefixes & PREFIX_OPSIZE )
	{
	   
		/* 99 
		 * DX:AX <- sign-extend of AX
		 * CWD 
		 */
		uint32_t sexd; 
		sexd = (int16_t)*c->reg16[ax];
		DWORD_UPPER_TO_WORD(*c->reg16[dx],sexd);
		DWORD_LOWER_TO_WORD(*c->reg16[ax],sexd);
		
	}
	else
	{
		/* 99 
		 * EDX:EAX <- sign-extend of EAX
		 * CDQ 
		 */
		uint64_t sexd; 
		sexd = (int32_t)c->reg[eax];
		QWORD_UPPER_TO_DWORD(c->reg[edx],sexd);
		QWORD_LOWER_TO_DWORD(c->reg[eax],sexd);

	}
	return 0;
}

