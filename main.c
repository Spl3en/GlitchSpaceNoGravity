#include <stdio.h>
#include <stdlib.h>

#include "./MemProc/MemProc.h"

bool key_toggle (int key)
{
	static char keystate[256] = {[0 ... 255] = 0};

	if (GetAsyncKeyState(key) < 0)
	{
		if (keystate[key] == 0)
		{
			keystate[key] = 1;
			return true;
		}
	}
	else {
		keystate[key] = 0;
	}

	return false;
}

int main ()
{
	MemProc *mp = memproc_new("glitchspace.exe", "Glitchspace");
	memproc_refresh_handle(mp);

	if (!mp->proc) {
		error("Glitchspace process not found");
	}
	else {
		info("Glitchspace process found");
		info("R = No gravity");
		info("F = Enable gravity");
	}

	DWORD text_section = mp->base_addr + 0x1000;
	unsigned int text_size = 0x7BF000;

	memproc_dump(mp, text_section, text_section + text_size);
	memproc_search(mp,
		(unsigned char []) {
			/*
				00820115   ║·  F30F7E00        movq xmm0, [qword ds:eax]
				00820119   ║·  8B7C24 14       mov edi, [dword ss:esp+14]
				here       ║·  F30F1107        movss [dword ds:edi], xmm0
				00820121   ║·  F30F7E40 08     movq xmm0, [qword ds:eax+8]
				00820126   ║·  660FD647 08     movq [qword ds:edi+8], xmm0
				0082012B   ║·  F30F7E40 10     movq xmm0, [qword ds:eax+10]
				00820130   ║·  660FD647 10     movq [qword ds:edi+10], xmm0
				00820135   ║·  8B40 18         mov eax, [dword ds:eax+18]
				00820138   ║·  8947 18         mov [dword ds:edi+18], eax
				0082013B   ║·  8B16            mov edx, [dword ds:esi]
				0082013D   ║·  8B42 08         mov eax, [dword ds:edx+8]
				00820140   ║·  8BCE            mov ecx, esi
				00820142   ║·  FFD0            call eax
			*/
			0xF3, 0x0F, 0x7E, 0x00,
			0x8B, 0x7C, 0x24, 0x14,
			0xF3, 0x0F, 0x11, 0x07,
			0xF3, 0x0F, 0x7E, 0x40, 0x08,
			0x66, 0x0F, 0xD6, 0x47, 0x08,
			0xF3, 0x0F, 0x7E, 0x40, 0x10,
			0x66, 0x0F, 0xD6, 0x47, 0x10,
			0x8B, 0x40, 0x18,
			0x89, 0x47, 0x18,
			0x8B, 0x16,
			0x8B, 0x42, 0x08,
			0x8B, 0xCE,
			0xFF, 0xD0
		},

		"xxxx"
		"xxxx"
		"????"
		"xxxxx"
		"xxxxx"
		"xxxxx"
		"xxxxx"
		"xxx"
		"xxx"
		"xx"
		"xxx"
		"xx"
		"xx",

		NULL, SEARCH_TYPE_BYTES
	);

	BbQueue *results = memproc_get_res(mp);

	MemBlock *block;

	if ((block = bb_queue_get_first(results))) {

		DWORD patch = block->addr + 0x8;
		info("Patch address found : 0x%x", (int) patch);

		bool running = true;
		while (running) {
			Sleep(1);

			if (key_toggle('X')) {
				running = false;
			}

			if (key_toggle('R')) {
				// Block gravity
				unsigned char noGravity [] = {0xF3, 0x0F, 0x11, 0x07};
				write_to_memory (mp->proc, noGravity, patch, sizeof(noGravity));
			}

			if (key_toggle('F')) {
				// Enable gravity
				unsigned char enableGravity [] = {0x66, 0x0F, 0xD6, 0x07};
				write_to_memory (mp->proc, enableGravity, patch, sizeof(enableGravity));
			}
		}
	}
	else {
		warning("No gravity patch not found...");
	}

    return 0;
}
