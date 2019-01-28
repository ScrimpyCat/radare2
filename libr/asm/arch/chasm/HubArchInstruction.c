/*
 *  Copyright (c) 2016, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "HubArchInstruction.h"
#include <stdio.h>
#include <string.h>

static const struct {
    const char *mnemonic;
    HKHubArchInstructionOperand operands[3];
    HKHubArchInstructionMemoryOperation memory;
} Instructions[64] = {
    { "add"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "add"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "mov"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "jz"      , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "sub"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "sub"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "mov"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "jnz"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "mul"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "mul"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "js"      , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "sdiv"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "sdiv"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "jns"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "udiv"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "udiv"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "jo"      , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "smod"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "smod"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "jno"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "umod"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "umod"    , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "send"    , { HKHubArchInstructionOperandR,   HKHubArchInstructionOperandR,   HKHubArchInstructionOperandM  }, HKHubArchInstructionMemoryOperationSSS },
    { "cmp"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationSS },
    { "cmp"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationSS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "send"    , { HKHubArchInstructionOperandR,   HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "shl"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "shl"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "send"    , { HKHubArchInstructionOperandI,   HKHubArchInstructionOperandR,   HKHubArchInstructionOperandM  }, HKHubArchInstructionMemoryOperationSSS },
    { "shr"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "shr"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "recv"    , { HKHubArchInstructionOperandR,   HKHubArchInstructionOperandM,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationSD },
    { "xor"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "xor"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { 0         , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "send"    , { HKHubArchInstructionOperandR,   HKHubArchInstructionOperandI,   HKHubArchInstructionOperandM  }, HKHubArchInstructionMemoryOperationSSS },
    { "or"      , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "or"      , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "neg"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationB },
    { "send"    , { HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "and"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "and"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandI,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationDS },
    { "not"     , { HKHubArchInstructionOperandRM,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationB },
    { "hlt"     , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "jsl"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jsge"    , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jsle"    , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jsg"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jul"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "juge"    , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jule"    , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "jug"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS },
    { "send"    , { HKHubArchInstructionOperandI,   HKHubArchInstructionOperandI,   HKHubArchInstructionOperandM  }, HKHubArchInstructionMemoryOperationSSS },
    { "recv"    , { HKHubArchInstructionOperandI,   HKHubArchInstructionOperandM,   HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationSD },
    { "nop"     , { HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA } },
    { "jmp"     , { HKHubArchInstructionOperandRel, HKHubArchInstructionOperandNA,  HKHubArchInstructionOperandNA }, HKHubArchInstructionMemoryOperationS }
};

static const struct {
    const char *mnemonic;
    uint8_t encoding;
} Registers[6] = {
    { "r0",      HKHubArchInstructionRegisterR0 },
    { "r1",      HKHubArchInstructionRegisterR1 },
    { "r2",      HKHubArchInstructionRegisterR2 },
    { "r3",      HKHubArchInstructionRegisterR3 },
    { "flags",   HKHubArchInstructionRegisterFlags },
    { "pc",      HKHubArchInstructionRegisterPC },
};

#pragma mark -

static inline uint64_t CCBitSet(uint64_t n)
{
    return n == 64 ? UINT64_MAX : ((UINT64_C(1) << n) - 1);
}

uint8_t HKHubArchInstructionDecode(const uint8_t *Data, int Len, HKHubArchInstructionState *Decoded)
{   
    HKHubArchInstructionState State = { .opcode = -1, .operand = { { .type = HKHubArchInstructionOperandNA }, { .type = HKHubArchInstructionOperandNA }, { .type = HKHubArchInstructionOperandNA } } };
    uint8_t Offset = 0;

    if (Offset >= Len)
    {
        State.opcode = -1;
        return Offset;
    }
    uint8_t Byte = Data[Offset++];
    
    uint8_t FreeBits = 2;
    const uint8_t Opcode = Byte >> 2;
    if (Instructions[Opcode].mnemonic)
    {
        State.opcode = Opcode;
        
        for (int Loop = 0; (Loop < 3) && (Instructions[Opcode].operands[Loop] != HKHubArchInstructionOperandNA); Loop++)
        {
            if (FreeBits == 0)
            {
                FreeBits = 8;
                if (Offset >= Len)
                {
                    State.opcode = -1;
                    break;
                }
                Byte = Data[Offset++];
            }
            
            if (Instructions[Opcode].operands[Loop] & HKHubArchInstructionOperandI)
            {
                uint8_t Value = (Byte & CCBitSet(FreeBits)) << (8 - FreeBits);
                if (FreeBits != 8)
                {
                    if (Offset >= Len)
                    {
                        State.opcode = -1;
                        break;
                    }
                    Byte = Data[Offset++];
                    Value |= Byte >> FreeBits;
                }
                
                else FreeBits = 0;
                
                State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandI, .value = Value };
            }
            
            else if (Instructions[Opcode].operands[Loop] & HKHubArchInstructionOperandRM)
            {
                uint8_t Reg = Byte & CCBitSet(FreeBits);
                if (FreeBits < 3)
                {
                    if (Offset >= Len)
                    {
                        State.opcode = -1;
                        break;
                    }
                    Byte = Data[Offset++];
                    Reg = (Reg << (3 - FreeBits)) | (Byte >> (8 - (3 - FreeBits)));
                    FreeBits = 8 - (3 - FreeBits);
                }
                
                else
                {
                    if (FreeBits > 3) Reg >>= (FreeBits - 3);
                    FreeBits -= 3;
                }
                
                if (Reg & (HKHubArchInstructionRegisterGeneralPurpose | HKHubArchInstructionRegisterSpecialPurpose))
                {
                    if (Instructions[Opcode].operands[Loop] & HKHubArchInstructionOperandR) State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandR, .reg = Reg };
                    else State.opcode = -1;
                }
                
                else if (Instructions[Opcode].operands[Loop] & HKHubArchInstructionOperandM)
                {
                    uint8_t Memory = Byte & CCBitSet(FreeBits);
                    if (FreeBits < 1)
                    {
                        if (Offset >= Len)
                        {
                            State.opcode = -1;
                            break;
                        }
                        Byte = Data[Offset++];
                        Memory = (Memory << (1 - FreeBits)) | (Byte >> (8 - (1 - FreeBits)));
                        FreeBits = 8 - (1 - FreeBits);
                    }
                    
                    else
                    {
                        if (FreeBits > 1) Memory >>= (FreeBits - 1);
                        FreeBits -= 1;
                    }
                    
                    Memory |= Reg << 1;
                    
                    switch (Memory)
                    {
                        case HKHubArchInstructionMemoryOffset:
                        {
                            uint8_t Value = (Byte & CCBitSet(FreeBits)) << (8 - FreeBits);
                            if (FreeBits != 8)
                            {
                                if (Offset >= Len)
                                {
                                    State.opcode = -1;
                                    break;
                                }
                                Byte = Data[Offset++];
                                Value |= Byte >> FreeBits;
                            }
                            
                            else FreeBits = 0;
                            
                            State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandM, .memory = { .type = Memory, .offset = Value } };
                            break;
                        }
                            
                        case HKHubArchInstructionMemoryRegister:
                            Reg = Byte & CCBitSet(FreeBits);
                            if (FreeBits < 2)
                            {
                                if (Offset >= Len)
                                {
                                    State.opcode = -1;
                                    break;
                                }
                                Byte = Data[Offset++];
                                Reg = (Reg << (2 - FreeBits)) | (Byte >> (8 - (2 - FreeBits)));
                                FreeBits = 8 - (2 - FreeBits);
                            }
                            
                            else
                            {
                                if (FreeBits > 2) Reg >>= (FreeBits - 2);
                                FreeBits -= 2;
                            }
                            
                            State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandM, .memory = { .type = Memory, .reg = Reg | HKHubArchInstructionRegisterGeneralPurpose } };
                            break;
                            
                        case HKHubArchInstructionMemoryRelativeOffset:
                        {
                            uint8_t Value = (Byte & CCBitSet(FreeBits)) << (8 - FreeBits);
                            if (FreeBits != 8)
                            {
                                if (Offset >= Len)
                                {
                                    State.opcode = -1;
                                    break;
                                }
                                Byte = Data[Offset++];
                                Value |= Byte >> FreeBits;
                            }
                            
                            else FreeBits = 0;
                            
                            Reg = Byte & CCBitSet(FreeBits);
                            if (FreeBits < 2)
                            {
                                if (Offset >= Len)
                                {
                                    State.opcode = -1;
                                    break;
                                }
                                Byte = Data[Offset++];
                                Reg = (Reg << (2 - FreeBits)) | (Byte >> (8 - (2 - FreeBits)));
                                FreeBits = 8 - (2 - FreeBits);
                            }
                            
                            else
                            {
                                if (FreeBits > 2) Reg >>= (FreeBits - 2);
                                FreeBits -= 2;
                            }
                            
                            State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandM, .memory = { .type = Memory, .relativeOffset = { .offset = Value, .reg = Reg | HKHubArchInstructionRegisterGeneralPurpose } } };
                            break;
                        }
                            
                        case HKHubArchInstructionMemoryRelativeRegister:
                            Reg = Byte & CCBitSet(FreeBits);
                            if (FreeBits < 4)
                            {
                                if (Offset >= Len)
                                {
                                    State.opcode = -1;
                                    break;
                                }
                                Byte = Data[Offset++];
                                Reg = (Reg << (4 - FreeBits)) | (Byte >> (8 - (4 - FreeBits)));
                                FreeBits = 8 - (4 - FreeBits);
                            }
                            
                            else
                            {
                                if (FreeBits > 4) Reg >>= (FreeBits - 4);
                                FreeBits -= 4;
                            }
                            
                            State.operand[Loop] = (HKHubArchInstructionOperandValue){ .type = HKHubArchInstructionOperandM, .memory = { .type = Memory, .relativeReg = { (Reg >> 2) | HKHubArchInstructionRegisterGeneralPurpose, (Reg & 3) | HKHubArchInstructionRegisterGeneralPurpose } } };
                            break;
                    }
                }
                
                else State.opcode = -1;
            }
        }
    }
    
    if (Decoded) *Decoded = State;
    
    return Offset;
}

void HKHubArchInstructionDisassemble(const HKHubArchInstructionState *State, char Disassembly[256])
{   
    Disassembly[0] = 0;
    if ((State->opcode != -1) && (Instructions[State->opcode].mnemonic))
    {
        strcpy(Disassembly, Instructions[State->opcode].mnemonic);
        
        for (int Loop = 0; (Loop < 3) && (State->operand[Loop].type != HKHubArchInstructionOperandNA); Loop++)
        {
            switch (State->operand[Loop].type)
            {
                case HKHubArchInstructionOperandI:
                {
                    char Hex[5];
                    snprintf(Hex, sizeof(Hex), State->operand[Loop].value ? "%#.2x" : "0x%#.2x", State->operand[Loop].value);
                    
                    strcat(Disassembly, Loop == 0 ? " " : ",");
                    strcat(Disassembly, Hex);
                    break;
                }
                    
                case HKHubArchInstructionOperandR:
                    if (State->operand[Loop].reg & HKHubArchInstructionRegisterGeneralPurpose)
                    {
                        strcat(Disassembly, Loop == 0 ? " " : ",");
                        strcat(Disassembly, Registers[State->operand[Loop].reg & HKHubArchInstructionRegisterGeneralPurposeIndexMask].mnemonic);
                    }
                    
                    else if (State->operand[Loop].reg & HKHubArchInstructionRegisterSpecialPurpose)
                    {
                        strcat(Disassembly, Loop == 0 ? " " : ",");
                        strcat(Disassembly, Registers[(State->operand[Loop].reg & HKHubArchInstructionRegisterSpecialPurposeIndexMask) + 4].mnemonic);
                    }
                    break;
                    
                case HKHubArchInstructionOperandM:
                {
                    char Memory[256] = {0};
                    switch (State->operand[Loop].memory.type)
                    {
                        case HKHubArchInstructionMemoryOffset:
                        {
                            char Hex[5];
                            snprintf(Hex, sizeof(Hex), State->operand[Loop].memory.offset ? "%#.2x" : "0x%#.2x", State->operand[Loop].memory.offset);
                            
                            strcat(Memory, "[");
                            strcat(Memory, Hex);
                            strcat(Memory, "]");
                            break;
                        }
                            
                        case HKHubArchInstructionMemoryRegister:
                            strcat(Memory, "[");
                            strcat(Memory, Registers[State->operand[Loop].memory.reg & HKHubArchInstructionRegisterGeneralPurposeIndexMask].mnemonic);
                            strcat(Memory, "]");
                            break;
                            
                        case HKHubArchInstructionMemoryRelativeOffset:
                        {
                            char Hex[5];
                            snprintf(Hex, sizeof(Hex), State->operand[Loop].memory.relativeOffset.offset ? "%#.2x" : "0x%#.2x", State->operand[Loop].memory.relativeOffset.offset);
                            
                            strcat(Memory, "[");
                            strcat(Memory, Hex);
                            strcat(Memory, "+");
                            strcat(Memory, Registers[State->operand[Loop].memory.relativeOffset.reg & HKHubArchInstructionRegisterGeneralPurposeIndexMask].mnemonic);
                            strcat(Memory, "]");
                            break;
                        }
                            
                        case HKHubArchInstructionMemoryRelativeRegister:
                            strcat(Memory, "[");
                            strcat(Memory, Registers[State->operand[Loop].memory.relativeReg[0] & HKHubArchInstructionRegisterGeneralPurposeIndexMask].mnemonic);
                            strcat(Memory, "+");
                            strcat(Memory, Registers[State->operand[Loop].memory.relativeReg[1] & HKHubArchInstructionRegisterGeneralPurposeIndexMask].mnemonic);
                            strcat(Memory, "]");
                            break;
                    }
                    
                    strcat(Disassembly, Loop == 0 ? " " : ",");
                    strcat(Disassembly, Memory);
                    break;
                }
                    
                default:
                    break;
            }
        }
    }
}
