/*
	Copyright (C) 2006 Guillaume Duhamel
	Copyright (C) 2006-2025 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "debug.h"

#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#define DEBUG_MKDIR(path) _mkdir(path)
#else
#define DEBUG_MKDIR(path) mkdir(path, 0755)
#endif
#include "MMU.h"
#include "armcpu.h"
#include "instructions.h"
#include "cp15.h"
#include "NDSSystem.h"
#include "utils/xstring.h"
#include "movie.h"

#ifdef HAVE_LUA
#include "lua-engine.h"
#endif

armcpu_t* TDebugEventData::cpu() { return procnum==0?&NDS_ARM9:&NDS_ARM7; }

TDebugEventData DebugEventData;
u32 debugFlag;

struct DebugInstructionTraceEntry
{
	u64 sequence;
	u64 ndsTimer;
	u32 frame;
	u32 procnum;
	u32 address;
	u32 instruction;
	u32 instructionSize;
	u32 nextInstruction;
	u32 cpsr;
	u32 registers[16];
	bool thumb;
	bool executed;
};

static const size_t DEBUG_TRACE_CAPACITY = 65536;
static DebugInstructionTraceEntry debugInstructionTrace[DEBUG_TRACE_CAPACITY];
static bool debugInstructionTraceEnabled = false;
static u64 debugInstructionTraceSequence = 0;
static size_t debugInstructionTraceWriteIndex = 0;
static size_t debugInstructionTraceCount = 0;
static std::string debugLastSnapshotPath;

// PACKET HACK VARS
FILE *log_ptr;	// File to store the dumped data
const u32 rc4_addr[2] = { 0x020986A8, 0x02098710 };

//DEBUG CONFIGURATION
const bool debug_acl = false;
const bool debug_cacheMiss = false;

static bool acl_check_access(u32 adr, u32 access) {

	//non-user modes get separate access handling, so check that here
	if(NDS_ARM9.CPSR.bits.mode != USR)
		access |= 1;
	
	if (armcp15_isAccessAllowed(&cp15, adr,access)==FALSE) {
		HandleDebugEvent(DEBUG_EVENT_ACL_EXCEPTION);
	}
	return true;
}

void HandleDebugEvent_ACL_Exception()
{
	printf("ACL EXCEPTION!\n");
	if(DebugEventData.memAccessType == MMU_AT_CODE)
		armcpu_exception(DebugEventData.cpu(),EXCEPTION_PREFETCH_ABORT);
	else if(DebugEventData.memAccessType == MMU_AT_DATA)
		armcpu_exception(DebugEventData.cpu(),EXCEPTION_DATA_ABORT);
}


static bool CheckRange(u32 adr, u32 min, u32 len)
{
	return (adr>=min && adr<min+len);
}

void DEBUG_setInstructionTraceEnabled(bool enabled)
{
	debugInstructionTraceEnabled = enabled;
}

bool DEBUG_isInstructionTraceEnabled()
{
	return debugInstructionTraceEnabled;
}

void DEBUG_clearInstructionTrace()
{
	debugInstructionTraceSequence = 0;
	debugInstructionTraceWriteIndex = 0;
	debugInstructionTraceCount = 0;
	memset(debugInstructionTrace, 0, sizeof(debugInstructionTrace));
}

void DEBUG_recordInstruction(const armcpu_t &cpu, u32 instructionSize)
{
	if (!debugInstructionTraceEnabled)
	{
		return;
	}
	
	DebugInstructionTraceEntry &entry = debugInstructionTrace[debugInstructionTraceWriteIndex];
	entry.sequence = debugInstructionTraceSequence++;
	entry.ndsTimer = nds_timer;
	entry.frame = (u32)currFrameCounter;
	entry.procnum = cpu.proc_ID;
	entry.address = cpu.instruct_adr;
	entry.instruction = cpu.instruction;
	entry.instructionSize = instructionSize;
	entry.nextInstruction = cpu.next_instruction;
	entry.cpsr = cpu.CPSR.val;
	entry.thumb = (cpu.CPSR.bits.T != 0);
	entry.executed = entry.thumb || (CONDITION(cpu.instruction) == 0x0E) || TEST_COND(CONDITION(cpu.instruction), CODE(cpu.instruction), cpu.CPSR);
	memcpy(entry.registers, cpu.R, sizeof(entry.registers));
	
	debugInstructionTraceWriteIndex = (debugInstructionTraceWriteIndex + 1) % DEBUG_TRACE_CAPACITY;
	if (debugInstructionTraceCount < DEBUG_TRACE_CAPACITY)
	{
		debugInstructionTraceCount++;
	}
}

const char* DEBUG_getLastSnapshotPath()
{
	return debugLastSnapshotPath.c_str();
}

static bool debug_mkdir_recursive(const char *path)
{
	if (path == NULL || path[0] == '\0')
	{
		return false;
	}
	
	char tempPath[4096];
	snprintf(tempPath, sizeof(tempPath), "%s", path);
	const size_t len = strlen(tempPath);
	if (len == 0)
	{
		return false;
	}
	
	if (tempPath[len - 1] == '/')
	{
		tempPath[len - 1] = '\0';
	}
	
	for (char *p = tempPath + 1; *p != '\0'; p++)
	{
		if (*p == '/')
		{
			*p = '\0';
			if (DEBUG_MKDIR(tempPath) != 0 && errno != EEXIST)
			{
				return false;
			}
			*p = '/';
		}
	}
	
	return (DEBUG_MKDIR(tempPath) == 0 || errno == EEXIST);
}

static void debug_json_string(FILE *fp, const char *text)
{
	fputc('"', fp);
	if (text != NULL)
	{
		for (const char *p = text; *p != '\0'; p++)
		{
			switch (*p)
			{
				case '\\': fputs("\\\\", fp); break;
				case '"': fputs("\\\"", fp); break;
				case '\n': fputs("\\n", fp); break;
				case '\r': fputs("\\r", fp); break;
				case '\t': fputs("\\t", fp); break;
				default: fputc(*p, fp); break;
			}
		}
	}
	fputc('"', fp);
}

static const char* debug_instruction_name(const DebugInstructionTraceEntry &entry)
{
	if (entry.thumb)
	{
		return thumb_instruction_names[entry.instruction >> 6];
	}
	
	return arm_instruction_names[INSTRUCTION_INDEX(entry.instruction)];
}

static void debug_write_cpu_json(FILE *fp, const char *name, const armcpu_t &cpu)
{
	fprintf(fp, "    \"%s\": {\n", name);
	fprintf(fp, "      \"pc\": \"0x%08X\",\n", cpu.R[15]);
	fprintf(fp, "      \"instruction_address\": \"0x%08X\",\n", cpu.instruct_adr);
	fprintf(fp, "      \"instruction\": \"0x%08X\",\n", cpu.instruction);
	fprintf(fp, "      \"next_instruction\": \"0x%08X\",\n", cpu.next_instruction);
	fprintf(fp, "      \"cpsr\": \"0x%08X\",\n", cpu.CPSR.val);
	fprintf(fp, "      \"thumb\": %s,\n", cpu.CPSR.bits.T ? "true" : "false");
	fprintf(fp, "      \"registers\": [");
	for (int i = 0; i < 16; i++)
	{
		fprintf(fp, "%s\"0x%08X\"", (i == 0) ? "" : ", ", cpu.R[i]);
	}
	fprintf(fp, "]\n");
	fprintf(fp, "    }");
}

static void debug_write_memory_window(FILE *fp, const char *name, int procnum, u32 center, u32 before, u32 size, bool trailingComma)
{
	u32 start = (center > before) ? (center - before) : 0;
	start &= ~0x0F;
	
	fprintf(fp, "    {\n");
	fprintf(fp, "      \"name\": ");
	debug_json_string(fp, name);
	fprintf(fp, ",\n");
	fprintf(fp, "      \"cpu\": \"ARM%d\",\n", procnum == ARMCPU_ARM9 ? 9 : 7);
	fprintf(fp, "      \"start\": \"0x%08X\",\n", start);
	fprintf(fp, "      \"size\": %u,\n", size);
	fprintf(fp, "      \"hex\": \"");
	
	std::vector<u8> buffer(size);
	MMU_DumpMemBlock((u8)procnum, start, size, &buffer[0]);
	for (u32 i = 0; i < size; i++)
	{
		fprintf(fp, "%02X", buffer[i]);
	}
	fprintf(fp, "\"\n");
	fprintf(fp, "    }%s\n", trailingComma ? "," : "");
}

static bool debug_write_binary_file(const std::string &path, const void *data, size_t size)
{
	FILE *fp = fopen(path.c_str(), "wb");
	if (fp == NULL)
	{
		return false;
	}
	
	const bool ok = (fwrite(data, 1, size, fp) == size);
	fclose(fp);
	return ok;
}

bool DEBUG_writeSnapshot(const char *directoryPath, const char *reason, bool includeFullMemory)
{
	const char *baseDirectory = directoryPath;
	if (baseDirectory == NULL || baseDirectory[0] == '\0')
	{
		baseDirectory = getenv("DESMUME_DEBUG_SNAPSHOT_DIR");
	}
	if (baseDirectory == NULL || baseDirectory[0] == '\0')
	{
		baseDirectory = ".";
	}
	
	if (!debug_mkdir_recursive(baseDirectory))
	{
		return false;
	}
	
	time_t now = time(NULL);
	struct tm nowLocal;
#ifdef _WIN32
	localtime_s(&nowLocal, &now);
#else
	localtime_r(&now, &nowLocal);
#endif
	char timestamp[32];
	strftime(timestamp, sizeof(timestamp), "%Y%m%d-%H%M%S", &nowLocal);
	
	char snapshotDirectory[4096];
	snprintf(snapshotDirectory, sizeof(snapshotDirectory), "%s/desmume-debug-%s-frame-%u", baseDirectory, timestamp, (u32)currFrameCounter);
	if (!debug_mkdir_recursive(snapshotDirectory))
	{
		return false;
	}
	
	std::string jsonPath = std::string(snapshotDirectory) + "/snapshot.json";
	FILE *fp = fopen(jsonPath.c_str(), "wb");
	if (fp == NULL)
	{
		return false;
	}
	
	NDSError lastError = NDS_GetLastError();
	fprintf(fp, "{\n");
	fprintf(fp, "  \"format\": \"desmume-debug-snapshot-v1\",\n");
	fprintf(fp, "  \"reason\": ");
	debug_json_string(fp, reason != NULL ? reason : "manual");
	fprintf(fp, ",\n");
	fprintf(fp, "  \"frame\": %u,\n", (u32)currFrameCounter);
	fprintf(fp, "  \"nds_timer\": \"%llu\",\n", (unsigned long long)nds_timer);
	fprintf(fp, "  \"trace_enabled\": %s,\n", debugInstructionTraceEnabled ? "true" : "false");
	fprintf(fp, "  \"trace_capacity\": %u,\n", (u32)DEBUG_TRACE_CAPACITY);
	fprintf(fp, "  \"trace_count\": %u,\n", (u32)debugInstructionTraceCount);
	fprintf(fp, "  \"last_error\": {\n");
	fprintf(fp, "    \"code\": %d,\n", lastError.code);
	fprintf(fp, "    \"tag\": %d,\n", lastError.tag);
	fprintf(fp, "    \"arm9_pc\": \"0x%08X\",\n", lastError.programCounterARM9);
	fprintf(fp, "    \"arm9_instruction\": \"0x%08X\",\n", lastError.instructionARM9);
	fprintf(fp, "    \"arm9_instruction_address\": \"0x%08X\",\n", lastError.instructionAddrARM9);
	fprintf(fp, "    \"arm7_pc\": \"0x%08X\",\n", lastError.programCounterARM7);
	fprintf(fp, "    \"arm7_instruction\": \"0x%08X\",\n", lastError.instructionARM7);
	fprintf(fp, "    \"arm7_instruction_address\": \"0x%08X\"\n", lastError.instructionAddrARM7);
	fprintf(fp, "  },\n");
	fprintf(fp, "  \"cpus\": {\n");
	debug_write_cpu_json(fp, "arm9", NDS_ARM9);
	fprintf(fp, ",\n");
	debug_write_cpu_json(fp, "arm7", NDS_ARM7);
	fprintf(fp, "\n  },\n");
	fprintf(fp, "  \"memory_windows\": [\n");
	debug_write_memory_window(fp, "arm9_pc", ARMCPU_ARM9, NDS_ARM9.instruct_adr, 0x40, 0x100, true);
	debug_write_memory_window(fp, "arm9_lr", ARMCPU_ARM9, NDS_ARM9.R[14], 0x40, 0x100, true);
	debug_write_memory_window(fp, "arm9_sp", ARMCPU_ARM9, NDS_ARM9.R[13], 0x80, 0x200, true);
	debug_write_memory_window(fp, "arm7_pc", ARMCPU_ARM7, NDS_ARM7.instruct_adr, 0x40, 0x100, true);
	debug_write_memory_window(fp, "arm7_lr", ARMCPU_ARM7, NDS_ARM7.R[14], 0x40, 0x100, true);
	debug_write_memory_window(fp, "arm7_sp", ARMCPU_ARM7, NDS_ARM7.R[13], 0x80, 0x200, false);
	fprintf(fp, "  ],\n");
	fprintf(fp, "  \"recent_instructions\": [\n");
	
	const size_t maxExport = std::min(debugInstructionTraceCount, (size_t)4096);
	const size_t firstIndex = (debugInstructionTraceWriteIndex + DEBUG_TRACE_CAPACITY - maxExport) % DEBUG_TRACE_CAPACITY;
	for (size_t i = 0; i < maxExport; i++)
	{
		const DebugInstructionTraceEntry &entry = debugInstructionTrace[(firstIndex + i) % DEBUG_TRACE_CAPACITY];
		fprintf(fp, "    {\n");
		fprintf(fp, "      \"seq\": \"%llu\",\n", (unsigned long long)entry.sequence);
		fprintf(fp, "      \"frame\": %u,\n", entry.frame);
		fprintf(fp, "      \"nds_timer\": \"%llu\",\n", (unsigned long long)entry.ndsTimer);
		fprintf(fp, "      \"cpu\": \"ARM%d\",\n", entry.procnum == ARMCPU_ARM9 ? 9 : 7);
		fprintf(fp, "      \"address\": \"0x%08X\",\n", entry.address);
		fprintf(fp, "      \"instruction\": \"0x%08X\",\n", entry.instruction);
		fprintf(fp, "      \"size\": %u,\n", entry.instructionSize);
		fprintf(fp, "      \"name\": ");
		debug_json_string(fp, debug_instruction_name(entry));
		fprintf(fp, ",\n");
		fprintf(fp, "      \"thumb\": %s,\n", entry.thumb ? "true" : "false");
		fprintf(fp, "      \"executed\": %s,\n", entry.executed ? "true" : "false");
		fprintf(fp, "      \"next_instruction\": \"0x%08X\",\n", entry.nextInstruction);
		fprintf(fp, "      \"cpsr\": \"0x%08X\",\n", entry.cpsr);
		fprintf(fp, "      \"registers\": [");
		for (int r = 0; r < 16; r++)
		{
			fprintf(fp, "%s\"0x%08X\"", (r == 0) ? "" : ", ", entry.registers[r]);
		}
		fprintf(fp, "]\n");
		fprintf(fp, "    }%s\n", (i + 1 < maxExport) ? "," : "");
	}
	fprintf(fp, "  ],\n");
	fprintf(fp, "  \"full_memory_files\": %s\n", includeFullMemory ? "[\"main_mem.bin\", \"arm9_dtcm.bin\", \"arm9_itcm.bin\", \"arm7_eram.bin\", \"shared_wram.bin\"]" : "[]");
	fprintf(fp, "}\n");
	fclose(fp);
	
	if (includeFullMemory)
	{
		debug_write_binary_file(std::string(snapshotDirectory) + "/main_mem.bin", MMU.MAIN_MEM, 0x800000);
		debug_write_binary_file(std::string(snapshotDirectory) + "/arm9_dtcm.bin", MMU.ARM9_DTCM, 0x4000);
		debug_write_binary_file(std::string(snapshotDirectory) + "/arm9_itcm.bin", MMU.ARM9_ITCM, 0x8000);
		debug_write_binary_file(std::string(snapshotDirectory) + "/arm7_eram.bin", MMU.ARM7_ERAM, 0x10000);
		debug_write_binary_file(std::string(snapshotDirectory) + "/shared_wram.bin", MMU.SWIRAM, 0x8000);
	}
	
	debugLastSnapshotPath = snapshotDirectory;
	printf("Debug snapshot written: %s\n", debugLastSnapshotPath.c_str());
	return true;
}

void HandleDebugEvent_Read()
{
	if(!debug_acl) return;
	if(DebugEventData.procnum != ARMCPU_ARM9) return; //acl only valid on arm9
	acl_check_access(DebugEventData.addr,CP15_ACCESS_READ);
}

void HandleDebugEvent_Write()
{
	// Disabled by default.
	// If you want to enable first you must know and update the address of the
	// RC4 algoritm function.
	return;

	// This method is called twice, so ommit one call.
	extern bool nds_debug_continuing[2];
	if (nds_debug_continuing[DebugEventData.procnum]) {
		nds_debug_continuing[DebugEventData.procnum] = false;
		return;
	}

	// RC4 encrypt / decrypt function
	//   R1: Pointer to data to operate (to decrypt or encrypt)
	//   R2: Size of this data
	if (DebugEventData.addr == rc4_addr[0] || DebugEventData.addr == rc4_addr[1]) {
		nds_debug_continuing[DebugEventData.procnum] = true;

		u32 addr = DebugEventData.addr;
		printf("WIFI: Call to RC4_ALGORITM\n");

		// Write log. Append current data
		// TODO: It needs to open the file each time, it could be slow.
		// An improvement could be opening the file at the start of the function
		// and closing at the end of the function.
		log_ptr = fopen("wifi_log.txt", "a");
		if (log_ptr != NULL)
		{
			// Create header
			time_t ti;
			time(&ti);
			tm* t = localtime(&ti);

			fprintf(log_ptr, "\n[%02d-%02d-%02d-%02d-%02d] %s of RC4_ALGORITM -----------\n", 
				t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
				(addr == rc4_addr[0]) ? "Start" : "End"
			);

			// Dump data
			int length   = DebugEventData.cpu()->R[2];
			int position = DebugEventData.cpu()->R[1] - 0x02000000;	// Relative to memory array
			fwrite(MMU.MAIN_MEM + position, sizeof(char), length, log_ptr);
			
			// End
			fprintf(log_ptr, "\n- THE END -----------------------------------------------\n");

			// Flush and close by the moment
			fflush(log_ptr);
			fclose(log_ptr);
		}
		else {
			printf("Error opening log file\n");
		}
	}
}

void HandleDebugEvent_Execute()
{
	//HACKY BREAKPOINTS!
	//extern bool nds_debug_continuing[2];
	//if(!nds_debug_continuing[DebugEventData.procnum]) //dont keep hitting the same breakpoint
	//{
	//	if((DebugEventData.addr & 0xFFFFFFF0) == 0x02000000)
	//	{
	//		void NDS_debug_break();
	//		NDS_debug_break();
	//	}
	//}
	if(!debug_acl) return;
	if(DebugEventData.procnum != ARMCPU_ARM9) return; //acl only valid on arm9
	acl_check_access(DebugEventData.addr,CP15_ACCESS_EXECUTE);
}

void HandleDebugEvent_CacheMiss()
{
	if(!debug_cacheMiss) return;
	extern int currFrameCounter;
	if(currFrameCounter<200) return;
	static FILE* outf = NULL;
	if(!outf) outf = fopen("c:\\miss.txt","wb");
	fprintf(outf,"%05d,%08X,%d\n",currFrameCounter,DebugEventData.addr,DebugEventData.size);
}

//------------------------------------------------
DebugStatistics DEBUG_statistics;

DebugStatistics::DebugStatistics()
{
}

DebugStatistics::InstructionHits::InstructionHits()
{
	memset(&arm,0,sizeof(arm));
	memset(&thumb,0,sizeof(thumb));
}


static DebugStatistics::InstructionHits combinedHits[2];

template<int proc, int which>
static bool debugStatsSort(int num1, int num2) {
	if(which==0) {
		if(combinedHits[proc].arm[num2] == combinedHits[proc].arm[num1]) return false;
		if(combinedHits[proc].arm[num1] == 0xFFFFFFFF) return false;
		if(combinedHits[proc].arm[num2] == 0xFFFFFFFF) return true;
		return combinedHits[proc].arm[num2] < combinedHits[proc].arm[num1];
	}
	else {
		if(combinedHits[proc].thumb[num2] == combinedHits[proc].thumb[num1]) return false;
		if(combinedHits[proc].thumb[num1] == 0xFFFFFFFF) return false;
		if(combinedHits[proc].thumb[num2] == 0xFFFFFFFF) return true;
		return combinedHits[proc].thumb[num2] < combinedHits[proc].thumb[num1];
	}
}

void DebugStatistics::print()
{
	//consolidate opcodes with the same names
	for(int i=0;i<2;i++) { 
		combinedHits[i] = DEBUG_statistics.instructionHits[i];

		for(int j=0;j<4096;j++) {
			if(combinedHits[i].arm[j] == 0xFFFFFFFF) 
				continue;
			std::string name = arm_instruction_names[j];
			for(int k=j+1;k<4096;k++) {
				if(combinedHits[i].arm[k] == 0xFFFFFFFF) 
					continue;
				if(name == arm_instruction_names[k]) {
					//printf("combining %s with %d and %d\n",name.c_str(),combinedHits[i].arm[j],combinedHits[i].arm[k]);
					combinedHits[i].arm[j] += combinedHits[i].arm[k];
					combinedHits[i].arm[k] = 0xFFFFFFFF;
				}
				
			}
		}

		for(int j=0;j<1024;j++) {
			if(combinedHits[i].thumb[j] == 0xFFFFFFFF) 
				continue;
			std::string name = thumb_instruction_names[j];
			for(int k=j+1;k<1024;k++) {
				if(combinedHits[i].thumb[k] == 0xFFFFFFFF) 
					continue;
				if(name == thumb_instruction_names[k]) {
					//printf("combining %s with %d and %d\n",name.c_str(),combinedHits[i].arm[j],combinedHits[i].arm[k]);
					combinedHits[i].thumb[j] += combinedHits[i].thumb[k];
					combinedHits[i].thumb[k] = 0xFFFFFFFF;
				}
				
			}
		}
	}

	InstructionHits sorts[2];
	for(int i=0;i<2;i++) {
		for(int j=0;j<4096;j++) sorts[i].arm[j] = j;
		for(int j=0;j<1024;j++) sorts[i].thumb[j] = j;
	}
	std::sort(sorts[0].arm, sorts[0].arm+4096, debugStatsSort<0,0>);
	std::sort(sorts[0].thumb, sorts[0].thumb+1024, debugStatsSort<0,1>);
	std::sort(sorts[1].arm, sorts[1].arm+4096, debugStatsSort<1,0>);
	std::sort(sorts[1].thumb, sorts[1].thumb+1024, debugStatsSort<1,1>);

	for(int i=0;i<2;i++) {
		printf("Top arm instructions for ARM%d:\n",7+i*2);
		for(int j=0;j<10;j++) {
			int val = sorts[i].arm[j];
			printf("%08d: %s\n", combinedHits[i].arm[val], arm_instruction_names[val]);
		}
		printf("Top thumb instructions for ARM%d:\n",7+i*2);
		for(int j=0;j<10;j++) {
			int val = sorts[i].thumb[j];
			printf("%08d: %s\n", combinedHits[i].thumb[val], thumb_instruction_names[val]);
		}
	}
}

void DebugStatistics::printSequencerExecutionCounters()
{
	for(int i=0;i<21;i++) printf("%06d ",sequencerExecutionCounters[i]);
	printf("\n");
}

void DEBUG_reset()
{
	//for now, just enable all debugging in developer builds
#ifdef DEVELOPER
	debugFlag = 1;
#endif

	DEBUG_Notify = DebugNotify();
	DEBUG_statistics = DebugStatistics();
	DEBUG_clearInstructionTrace();
	printf("DEBUG_reset: %p\n",&DebugStatistics::print); //force a reference to this function
}

static void DEBUG_dumpMemory_fill(EMUFILE &fp, u32 size)
{
	static std::vector<u8> buf;
	buf.resize(size);
	memset(&buf[0],0,size);
	fp.fwrite(&buf[0],size);
}

void DEBUG_dumpMemory(EMUFILE &fp)
{
	fp.fseek(0x000000,SEEK_SET); fp.fwrite(MMU.MAIN_MEM,0x800000); //arm9 main mem (8192K)
	fp.fseek(0x900000,SEEK_SET); fp.fwrite(MMU.ARM9_DTCM,0x4000); //arm9 DTCM (16K)
	fp.fseek(0xA00000,SEEK_SET); fp.fwrite(MMU.ARM9_ITCM,0x8000); //arm9 ITCM (32K)
	fp.fseek(0xB00000,SEEK_SET); fp.fwrite(MMU.ARM9_LCD,0xA4000); //LCD mem 656K
	fp.fseek(0xC00000,SEEK_SET); fp.fwrite(MMU.ARM9_VMEM,0x800); //OAM
	fp.fseek(0xD00000,SEEK_SET); fp.fwrite(MMU.ARM7_ERAM,0x10000); //arm7 WRAM (64K)
	fp.fseek(0xE00000,SEEK_SET); fp.fwrite(MMU.ARM7_WIRAM,0x10000); //arm7 wifi RAM ?
	fp.fseek(0xF00000,SEEK_SET); fp.fwrite(MMU.SWIRAM,0x8000); //arm9/arm7 shared WRAM (32KB)
}

//----------------------------------------------------

std::vector<Logger *> Logger::channels;


static void defaultCallback(const Logger& logger, const char * message) {
	logger.getOutput() << message;
}

Logger::Logger() {
	out = &std::cout;
	callback = defaultCallback;
	flags = 0;
}

Logger::~Logger() {
	for(int i=0;i<(int)channels.size();i++)
		delete channels[i];
}

void Logger::vprintf(const char * format, va_list l, const char * file, unsigned int line) {
	char buffer[1024];
	char * cur = buffer;

	if (flags & Logger::FILE) cur += sprintf(cur, "%s:", file);
	if (flags & Logger::LINE) cur += sprintf(cur, "%d:", line);
	if (flags) cur += sprintf(cur, " ");

	::vsnprintf(cur, 1024, format, l);
	callback(*this, buffer);
}

void Logger::setOutput(std::ostream * o) {
	out = o;
}

void Logger::setCallback(void (*cback)(const Logger& logger, const char * message)) {
	callback = cback;
}

void Logger::setFlag(unsigned int flag) {
	this->flags = flag;
}

void Logger::fixSize(unsigned int channel) {
	while(channel >= channels.size()) {
		channels.push_back(new Logger());
	}
}

std::ostream& Logger::getOutput() const {
	return *out;
}

void Logger::log(unsigned int channel, const char * file, unsigned int line, const char * format, ...) {
	fixSize(channel);

	va_list l;
	va_start(l, format);
	channels[channel]->vprintf(format, l, file, line);
	va_end(l);
}

void Logger::log(unsigned int channel, const char * file, unsigned int line, std::ostream& os) {
	fixSize(channel);

	channels[channel]->setOutput(&os);
}

void Logger::log(unsigned int channel, const char * file, unsigned int line, unsigned int flag) {
	fixSize(channel);

	channels[channel]->setFlag(flag);
}

void Logger::log(unsigned int channel, const char * file, unsigned int line, void (*callback)(const Logger& logger, const char * message)) {
	fixSize(channel);

	channels[channel]->setCallback(callback);
}

void IdeasLog(armcpu_t* cpu)
{
	u32 adr = cpu->R[0];
	for(;;) {
		u8 c = _MMU_read08(cpu->proc_ID, MMU_AT_DEBUG, adr);
		adr++;
		if(!c) break;
		printf("%c",c);
	}
	
	//don't emit a newline. that is a pain in the butt.
	
	fflush(stdout);
}

void NocashMessage(armcpu_t* cpu, int offset)
{
	u32 adr = cpu->instruct_adr + offset;

	std::string todo;
	for(;;) {
		u8 c = _MMU_read08(cpu->proc_ID, MMU_AT_DEBUG, adr);
		adr++;
		if(!c) break;
		todo.push_back(c);
	}

	//r0,r1,r2,...,r15  show register content (displayed as 32bit Hex number)
	//sp,lr,pc          alias for r13,r14,r15
	//scanline          show current scanline number
	//frame             show total number of frames since coldboot
	//totalclks         show total number of clock cycles since coldboot
	//lastclks          show number of cycles since previous lastclks (or zeroclks)
	//zeroclks          resets the 'lastclks' counter

	//this is very inefficiently coded!
	char tmp[100];
	todo = mass_replace(todo, "%sp%", "%r13%");
	todo = mass_replace(todo, "%lr%", "%r14%");
	todo = mass_replace(todo, "%pc%", "%r15%");
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 0]);       todo = mass_replace(todo,  "%r0%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 1]);       todo = mass_replace(todo,  "%r1%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 2]);       todo = mass_replace(todo,  "%r2%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 3]);       todo = mass_replace(todo,  "%r3%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 4]);       todo = mass_replace(todo,  "%r4%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 5]);       todo = mass_replace(todo,  "%r5%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 6]);       todo = mass_replace(todo,  "%r6%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 7]);       todo = mass_replace(todo,  "%r7%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 8]);       todo = mass_replace(todo,  "%r8%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[ 9]);       todo = mass_replace(todo,  "%r9%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[10]);       todo = mass_replace(todo, "%r10%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[11]);       todo = mass_replace(todo, "%r11%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[12]);       todo = mass_replace(todo, "%r12%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[13]);       todo = mass_replace(todo, "%r13%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[14]);       todo = mass_replace(todo, "%r14%", tmp);
	snprintf(tmp, sizeof(tmp), "%08X", cpu->R[15]);       todo = mass_replace(todo, "%r15%", tmp);
	snprintf(tmp, sizeof(tmp),   "%d", nds.VCount);       todo = mass_replace(todo, "%scanline%", tmp);
	snprintf(tmp, sizeof(tmp),   "%d", currFrameCounter); todo = mass_replace(todo, "%frame%", tmp);
	snprintf(tmp, sizeof(tmp), "%lld", nds_timer);        todo = mass_replace(todo, "%totalclks%", tmp);

	printf("%s",todo.c_str());
	fflush(stdout);
}

//-------
DebugNotify DEBUG_Notify;

//enable bits arent being used right now.
//if you want exhaustive logging, move the print before the early return (or comment the early return)

//the intent of this system is to provide a compact dialog box showing which debug notifies have been
//triggered in this frame (with a glowing LED!) and which debug notifies have been triggered EVER
//which can be cleared, like a clip indicator in an audio tool.
//obviously all this isnt implemented yet.

void DebugNotify::NextFrame()
{
#ifdef DEVELOPER
	pingBits.reset();
#endif
}

void DebugNotify::ReadBeyondEndOfCart(u32 addr, u32 romsize)
{
#ifdef DEVELOPER
	if(!ping(DEBUG_NOTIFY_READ_BEYOND_END_OF_CART)) return;
	INFO("Reading beyond end of cart! ... %08X >= %08X\n",addr,romsize);
#endif
}

bool DebugNotify::ping(EDEBUG_NOTIFY which)
{
	bool wasPinged = pingBits[(int)which];
	pingBits[(int)which] = true;
	return !wasPinged;
}
