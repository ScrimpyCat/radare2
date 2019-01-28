/* radare2 - 2-clause BSD - Copyright 2019 - ScrimpyCat */

#include <stdio.h>
#include <string.h>
#include <r_types.h>
#include <r_lib.h>
#include <r_asm.h>
#include "../arch/chasm/HubArchInstructionType.h"
#include "../arch/chasm/HubArchInstruction.h"

static int disassemble(RAsm *a, RAsmOp *op, const ut8 *buf, int len) {
	HKHubArchInstructionState State;
	op->size = HKHubArchInstructionDecode (buf, len, &State);

	if (State.opcode == -1) return -1;

	char Disassembly[256];
	HKHubArchInstructionDisassemble (&State, Disassembly);
	r_strbuf_set (&op->buf_asm, Disassembly);
	return op->size;
}

RAsmPlugin r_asm_plugin_chasm = {
	.name = "chasm",
	.author = "ScrimpyCat",
	.license = "BSD",
	.version = "0.0.0",
	.desc = "Computing HUB disassembly plugin",
	.arch = "chasm",
	.bits = 8,
	.endian = R_SYS_ENDIAN_NONE,
	.disassemble = &disassemble
};

#ifndef CORELIB
R_API RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_ASM,
	.data = &r_asm_plugin_chasm,
	.version = R2_VERSION
};
#endif
