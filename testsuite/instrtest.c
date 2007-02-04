#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>


#include <emu/emu.h>
#include <emu/emu_memory.h>
#include <emu/emu_cpu.h>
#include <emu/emu_log.h>

#define FAILED "\033[31;1mfailed\033[0m"
#define SUCCESS "\033[32;1msuccess\033[0m"

int verbose;

static const char *regm[] = {
	"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};


struct instr_test
{
	const char *instr;

	uint8_t  *code;
	uint16_t codesize;

	struct 
	{
		uint32_t reg[8];
		uint32_t		mem_state[2];
	} in_state;

	struct 
	{
		uint32_t reg[8];
		uint32_t		mem_state[2];
	}out_state;
};


struct instr_test tests[] = 
{
/*	{
		.instr = "instr",
		.in_state.reg  = {0,0,0,0,0,0,0,0 },
		.in_state.mem_state = {0, 0},
		.out_state.reg  = {0,0,0,0,0,0,0,0 },
		.out_state.mem_state = {0, 0},
	},*/
	{
		.instr = "add ah,al",
		.in_state.reg  = {0xff01,0,0,0,0,0,0,0 },
		.in_state.mem_state = {0, 0},
		.out_state.reg  = {0x01,0,0,0,0,0,0,0 },
		.out_state.mem_state = {0, 0},
	},
	{
		.instr = "add ch,dl",
		.in_state.reg  = {0,0x1000,0x20,0,0,0,0,0 },
		.in_state.mem_state = {0, 0},
		.out_state.reg  = {0,0x3000,0x20,0,0,0,0,0 },
		.out_state.mem_state = {0, 0},
	},
	{
		.instr = "add [ecx],al",
		.in_state.reg  = {0x10,0x40000,0,0,0,0,0,0 },
		.in_state.mem_state = {0x40000, 0x10101010},
		.out_state.reg  = {0x10,0x40000,0,0,0,0,0,0 },
		.out_state.mem_state = {0x40000, 0x10101020},
	},
};

int prepare()
{
	int i;
	for (i=0;i<sizeof(tests)/sizeof(struct instr_test);i++)
	{
		const char *use = "USE32\n";
		FILE *f=fopen("/tmp/foo.S","w+");

		fwrite(use,strlen(use),1,f);
		fwrite(tests[i].instr,1,strlen(tests[i].instr),f);
		fclose(f);
		system("cd /tmp/; nasm foo.S");
		f=fopen("/tmp/foo","r");
		fseek(f,0,SEEK_END);

		tests[i].codesize = ftell(f);
		tests[i].code = malloc(tests[i].codesize);
		fseek(f,0,SEEK_SET);
		fread(tests[i].code,1,tests[i].codesize,f);
		fclose(f);

		unlink("/tmp/foo.S");
		unlink("/tmp/foo");
	}
	return 0;
}


int test()
{
	int i=0;

	for (i=0;i<sizeof(tests)/sizeof(struct instr_test);i++)
	{
		int failed = 0;


		printf("testing '%s' \t",tests[i].instr);
		int j=0;

/*		printf("'");

		for (j=0;j<tests[i].codesize;j++)
		{
			printf("%02x ",tests[i].code[j]);
		}
		printf("'");
*/
		struct emu *e = emu_new();
		struct emu_cpu *cpu = emu_cpu_get(e);

		// for i in eax  ecx edx ebx esp ebp esi edi; do echo "emu_cpu_reg32_set(cpu, $i, tests[i].start.$i );" ; done
		for ( j=0;j<8;j++ )
		{
			emu_cpu_reg32_set(cpu,j ,tests[i].in_state.reg[j]);
		}
   	


		struct emu_memory *mem = emu_memory_get(e);
		int static_offset = 4711;
		for( j = 0; j < tests[i].codesize; j++ )
		{
			emu_memory_write_byte(mem, static_offset+j, tests[i].code[j]);
		}

		if( tests[i].in_state.mem_state[0] != 0 )
		{
			emu_memory_write_dword(mem, tests[i].in_state.mem_state[0], tests[i].in_state.mem_state[1]);
		}

/*		emu_memory_write_byte(mem, static_offset+i, '\xcc');*/
		emu_cpu_eip_set(emu_cpu_get(e), static_offset);
		
		int ret = emu_cpu_step(emu_cpu_get(e));
		
		if( ret != 0 )
		{
			printf("cpu error %s\n", emu_strerror(e));
		}

		// for i in eax  ecx edx ebx esp ebp esi edi; do echo "if (emu_cpu_reg32_get(cpu, $i) ==  tests[i].stopp.$i ) { printf(\"\t $i \"SUCCESS); } else { printf(\"\t $i "FAILED" %i expected %i\n\",emu_cpu_reg32_get(cpu, $i),tests[i].stopp.$i); }" ; done

		for ( j=0;j<8;j++ )
		{
			if ( emu_cpu_reg32_get(cpu, j) ==  tests[i].out_state.reg[j] )
			{
				if (verbose == 1)
					printf("\t %s "SUCCESS"\n",regm[j]);
			} else
			{
				printf("\t %s "FAILED" got %i expected %i\n",regm[j],emu_cpu_reg32_get(cpu, j),tests[i].out_state.reg[j]);
				failed = 1;
			}
		}

		uint32_t value;

		if ( tests[i].out_state.mem_state[0] != 0 )
		{
			if ( emu_memory_read_dword(mem,tests[i].out_state.mem_state[0],&value) == 0 )
			{
				if ( value == tests[i].out_state.mem_state[1] )
				{
					if (verbose == 1)
						printf("\t memory "SUCCESS"\n");
				}
				else
				{
					printf("\t memory "FAILED" got %08x expected %08x\n",value, tests[i].out_state.mem_state[1]);
					failed = 1;
				}

			} else
			{
				printf("\t"FAILED" emu says: '%s' when accessing %08x\n", strerror(emu_errno(e)),tests[i].out_state.mem_state[0]);
				failed = 1;
			}

		}
		if (failed == 0)
		{
			printf(SUCCESS"\n");
		}
		emu_free(e);
	}
	return 0;
}

void cleanup()
{
	int i;
	for (i=0;i<sizeof(tests)/sizeof(struct instr_test);i++)
    	if (tests[i].code != NULL)
    		free(tests[i].code);
		
}

int main()
{
	verbose = 0;
	prepare();
	test();
	cleanup();

	return 0;
}
