#ifndef HAVE_EMU_H
#define HAVE_EMU_H

#include <emu/log.h>

struct emu
{
	int i;

	struct emu_logging log;
};


struct emu *emu_new();

#endif // HAVE_EMU_H
