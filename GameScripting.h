#pragma once

#define MAX_SCRIPT_VARS 16              // Size of our variable saving array
#define MAX_SCRIPT_SIZE 255             // Size of ScriptBuf - Max is really (2+(13*5))

struct GAME_SCRIPT_THREAD               // 0xE0 bytes total.
{
	BYTE    Pad1[0x14];                     // 0x00 [0x14]
	DWORD   dwScriptIP;                     // 0x14 [0x04]
	BYTE    Pad2[0x24];                     // 0x18 [0x24]
	DWORD   dwLocalVar[18];         // 0x3C [0x48]
	BYTE    Pad3[0x41];                     // 0x84 [0x41]
	DWORD   dwIfFlag;                       // 0xC5 [0x04]
	BYTE    Pad4[0xF];                      // 0xC9 [0x0F]
	DWORD   dwIndexSomething;       // 0xD8 [0x04]  (indexes into 0xA49960)
	BYTE    bHasLocalCopy;          // 0xDC [0x01]  (has local copy of something, if 1, then uses .data:00A48960 CTheScripts_LocalCopy instead of var0)
	BYTE    Pad5[0x3];                      // 0xDD [0x03]
};

struct SCRIPT_COMMAND                   //      Params
{       //              i = integer
	WORD    OpCode; //              f = float
	char    Params[MAX_SCRIPT_VARS];        //      v = variable
};      //              s = string
void                                    InitScripting();
int                                     ScriptCommand(const SCRIPT_COMMAND *pScriptCommand, ...);
const SCRIPT_COMMAND player_perform_animation_at_x_times_normal_rate = { 0x0393, "isf" };