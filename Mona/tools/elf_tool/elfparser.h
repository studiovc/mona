/*!
  \file   elfparser.cpp
  \brief  Mona OS. ELF relocation parser

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/05/02 update:$Date$
*/

#ifndef __MONA_ELF_PARSER_H__
#define __MONA_ELF_PARSER_H__

#include "elf.h"

class ELFParser
{
public:
    ELFParser();
    virtual ~ELFParser();

public:
    bool set(byte* elf, dword size, dword toAddress = 0);
    int getType();
    int parse();
    bool load(byte* image);

    inline dword getStartAddr()  const { return this->startAddr; }
    inline dword getEndAddr()    const { return this->endAddr; }
    inline dword getImageSize()  const { return this->imageSize; }
    inline dword getEntryPoint() const { return this->header->entrypoint; }

private:
    const char* getSectionName(dword index);
    const char* getSymbolName(dword index);

private:
    byte* elf;
    ELFHeader* header;
    ELFProgramHeader* pheader;
    ELFSectionHeader* sheader;
    ELFSymbolEntry* symbols;
    dword sectionNames, symbolNames;
    dword startAddr, endAddr;
    dword imageSize;
    dword toAddress;

public:
    enum
    {
        ERR_SIZE           = 6,
        TYPE_NOT_ELF       = 5,
        TYPE_NOT_SUPPORTED = 4,
        TYPE_RELOCATABLE   = 2,
        TYPE_EXECUTABLE    = 1
    };

    enum
    {
        PT_NULL    = 0,
        PT_LOAD    = 1,
        PT_DYNAMIC = 2,
        PT_INTERP  = 3,
        PT_NOTE    = 4,
        PT_SHLIB   = 5,
        PT_PHDR    = 6
    };

    enum
    {
        SHT_NULL     = 0,
        SHT_PROGBITS = 1,
        SHT_SYMTAB   = 2,
        SHT_STRTAB   = 3,
        SHT_RELA     = 4,
        SHT_HASH     = 5,
        SHT_DYNAMIC  = 6,
        SHT_NOTE     = 7,  /* OS defined           */
        SHT_NOBITS   = 8,  /* bss                  */
        SHT_REL      = 9,
        SHT_SHLIB    = 10, /* reserved. DO NOT USE */
        SHT_DYNSYM   = 11
    } SectionType;

    enum
    {
        WRITABLE   = 0x01,
        ALLOC      = 0x02,
        EXECUTABLE = 0x04
    } SectionFlags;

    enum
    {
        R_386_NONE     = 0,
        R_386_32       = 1,
        R_386_PC32     = 2,
        R_386_GOT32    = 3,
        R_386_PLT32    = 4,
        R_386_COPY     = 5 ,
        R_386_GLOB_DAT = 6,
        R_386_JMP_SLOT = 7,
        R_386_RELATIVE = 8,
        R_386_GOTOFF   = 9,
        R_386_GOTPC    = 10
    };

};

#endif  // __MONA_ELF_PARSER_H__
