/* is_dgsh.c --  Detect whether an executable is dgsh-compatible */

/* Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ctype.h>
#ifdef __APPLE__
#include <mach-o/loader.h>
#include <mach-o/swap.h>
#include <math.h>
#else
#include <elf.h>
#endif
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define DGSH_NAME "DSpinellis/dgsh"

#define MAX_LINE_LEN 1024

#ifdef __APPLE__
void *
load_bytes(FILE *obj_file, int offset, int size)
{
  void *buf = calloc(1, size);
  fseek(obj_file, offset, SEEK_SET);
  fread(buf, size, 1, obj_file);
  return buf;
}


int
has_macho_sections(FILE *obj_file, uint32_t type, int offset, int is_swap,
		uint32_t nsections)
{
  int actual_offset = offset;
  for (int  i = 0; i < nsections; i++) {
    if (type == LC_SEGMENT_64) {
      int struct_size = sizeof(struct section_64);
      struct section_64 *section = load_bytes(obj_file, actual_offset,
                                           struct_size);
 
      if (is_swap) {
        swap_section_64(section, nsections, 0);
      }

      if (strcmp(section->segname, ".note.ident")) {
        free(section);
        continue;
      }

      char *section_data = load_bytes(obj_file, section->offset, section->size);

      if (section->size == sizeof(DGSH_NAME) &&
		 memcmp(section_data, DGSH_NAME, sizeof(DGSH_NAME)) == 0) {
             free(section);
	     free(section_data);
	     return 1;
      }
      actual_offset += struct_size;

      free(section);
      free(section_data);
    } else if (type == LC_SEGMENT) {
      int struct_size = sizeof(struct section);
      struct section *section = load_bytes(obj_file, actual_offset, struct_size);

      if (is_swap) {
        swap_section(section, nsections, 0);
      }

      if (strcmp(section->segname, ".note.ident")) {
        free(section);
        continue;
      }

      char *section_data = load_bytes(obj_file, section->offset, section->size);

      if (section->size == sizeof(DGSH_NAME) &&
		 memcmp(section_data, DGSH_NAME, sizeof(DGSH_NAME)) == 0) {
             free(section);
	     free(section_data);
	     return 1;
      }
      actual_offset += struct_size;

      free(section);
      free(section_data);
    }
  }
  return 0;
}


int
has_macho_segment_commands(FILE *obj_file, int offset, int is_swap, uint32_t ncmds)
{
  int r;
  int actual_offset = offset;
  for (int  i = 0; i < ncmds; i++) {
    struct load_command *cmd = load_bytes(obj_file, actual_offset,
                                          sizeof(struct load_command));
    if (is_swap) {
      swap_load_command(cmd, 0);
    }

    if (cmd->cmd == LC_SEGMENT_64) {
      int struct_size = sizeof(struct segment_command_64);
      struct segment_command_64 *segment = load_bytes(obj_file, actual_offset,
                                                      struct_size);
      if (is_swap) {
        swap_segment_command_64(segment, 0);
      }

      r = has_macho_sections(obj_file, cmd->cmd, actual_offset + struct_size, is_swap,
                    segment->nsects);

      free(segment);
    } else if (cmd->cmd == LC_SEGMENT) {
      int struct_size = sizeof(struct segment_command);
      struct segment_command *segment = load_bytes(obj_file, actual_offset,
                                                   struct_size);
      if (is_swap) {
        swap_segment_command(segment, 0);
      }

      if (strcmp(segment->segname, ".note.ident")) {
        free(segment);
        continue;
      }

      r = has_macho_sections(obj_file, cmd->cmd, actual_offset + struct_size, is_swap,
                    segment->nsects);

      free(segment);
    }

    actual_offset += cmd->cmdsize;

    free(cmd);

    if (r == 1)
        return r;
  }
  return 0;
}


int
has_macho_header(FILE *obj_file, int offset, int is_64, int is_swap)
{
  uint32_t ncmds;
  int load_commands_offset = offset;

  if (is_64) {
    int header_size = sizeof(struct mach_header_64);
    struct mach_header_64 *header = load_bytes(obj_file, offset, header_size);
    if (is_swap) {
      swap_mach_header_64(header, 0);
    }
    ncmds = header->ncmds;
    load_commands_offset += header_size;

    free(header);
  } else {
    int header_size = sizeof(struct mach_header);
    struct mach_header *header = load_bytes(obj_file, offset, header_size);
    if (is_swap) {
      swap_mach_header(header, 0);
    }

    ncmds = header->ncmds;
    load_commands_offset += header_size;

    free(header);
  }

  return has_macho_segment_commands(obj_file, load_commands_offset, is_swap, ncmds);
}


int should_swap_bytes(uint32_t magic) {
  return magic == MH_CIGAM || magic == MH_CIGAM_64;
}

int
is_magic_64(uint32_t magic)
{
  return magic == MH_MAGIC_64 || magic == MH_CIGAM_64;
}


uint32_t
read_magic(FILE *obj_file, int offset)
{
  uint32_t magic;
  fseek(obj_file, offset, SEEK_SET);
  fread(&magic, sizeof(uint32_t), 1, obj_file);
  return magic;
}


int
has_macho_segments(FILE *obj_file)
{
  uint32_t magic = read_magic(obj_file, 0);
  int is_64 = is_magic_64(magic);
  int is_swap = should_swap_bytes(magic);
  return has_macho_header(obj_file, 0, is_64, is_swap);
}


int
is_macho_dgsh_program(const char *path)
{
  FILE *obj_file = fopen(path, "rb");
  int r = has_macho_segments(obj_file);
  fclose(obj_file);
  return r;
}
#else


/*
 * Return true if the provided ELF data contain a DGSH note section
 */
static int
has_dgsh_section_32(Elf32_Shdr *shdr, char *strTab, int shNum, uint8_t *data)
{
  int   i;
  Elf32_Nhdr *note;

  for (i = 0; i < shNum; i++)
    {
      if (strcmp(&strTab[shdr[i].sh_name], ".note.ident"))
	continue;
      note = (Elf32_Nhdr *)(data + shdr[i].sh_offset);
      if (note->n_namesz == sizeof(DGSH_NAME) && memcmp(note + 1, DGSH_NAME, sizeof(DGSH_NAME)) == 0)
	return 1;
  }
  return 0;
}

static int
has_dgsh_section_64(Elf64_Shdr *shdr, char *strTab, int shNum, uint8_t *data)
{
  int   i;
  Elf64_Nhdr *note;

  for (i = 0; i < shNum; i++)
    {
      if (strcmp(&strTab[shdr[i].sh_name], ".note.ident"))
	continue;
      note = (Elf64_Nhdr *)(data + shdr[i].sh_offset);
      if (note->n_namesz == sizeof(DGSH_NAME) && memcmp(note + 1, DGSH_NAME, sizeof(DGSH_NAME)) == 0)
	return 1;
  }
  return 0;
}


/* Return true if the ELF program pointed by data is dgsh-compatible */
static int
is_elf_dgsh_program(void *data)
{
  Elf32_Ehdr *elf;
  char *strtab;
  int arch;

  elf = (Elf32_Ehdr *)data;

  /* Check ELF magic number */
  if (memcmp(elf->e_ident, ELFMAG, SELFMAG) != 0)
    return 0;

  arch = elf->e_ident[EI_CLASS];
  if (arch == 1)	/* 32 bit */
    {
      Elf32_Shdr *shdr;
      shdr = (Elf32_Shdr *)(data + elf->e_shoff);
      strtab = (char *)(data + shdr[elf->e_shstrndx].sh_offset);
      return has_dgsh_section_32(shdr, strtab, elf->e_shnum, (uint8_t*)data);
    }
  else if (arch == 2)	/* 64 bit */
    {
      Elf64_Ehdr *elf64 = (Elf64_Ehdr *)elf;
      Elf64_Shdr *shdr;
      shdr = (Elf64_Shdr *)(data + elf64->e_shoff);
      strtab = (char *)(data + shdr[elf64->e_shstrndx].sh_offset);
      return has_dgsh_section_64(shdr, strtab, elf64->e_shnum, (uint8_t*)data);
    }
  else
    return 0;
}
#endif /* __APPLE__ */

/*
 * Return the position of string needle in the first line of the
 * (non-empty) haystack looking in up to hlen characters.
 */
static const char *
linenstr(const char *haystack, const char *needle, size_t hlen)
{
  size_t nlen = strlen(needle);
  while (nlen < hlen-- && *haystack != '\n')
    {
      if (memcmp(haystack, needle, nlen) == 0)
	return haystack;
      haystack++;
    }
  return (NULL);
}

/* Return true if a script's second line contains the sequence #!dgsh */
static int
is_magic_script_dgsh_program(const char *data, size_t len)
{
  const char magic[] = "#!dgsh";
  const char *line2 = memchr(data, '\n', len);

  if (line2 == NULL)
    return 0;
  len -= ++line2 - data;
  if (len < sizeof(magic))
    return 0;
  return (memcmp(line2, magic, sizeof(magic) - 1) == 0);
}


/* Return true if the ELF program pointed by data is dgsh-compatible */
static int
is_script_dgsh_program(void *data, size_t len)
{
  len = len < MAX_LINE_LEN ? len : MAX_LINE_LEN;

  return (linenstr(data, "dgsh-wrap", len) ||
  	  linenstr(data, "--dgsh", len) ||
  	  linenstr(data, "env dgsh", len) ||
	  is_magic_script_dgsh_program(data, len));
}


int
is_dgsh_program(const char *path)
{
  void *data;
  int fd;
  int r;
  off_t file_size;


  fd = open(path, O_RDONLY);
  if (fd == -1)
    return 0;
  file_size = lseek(fd, 0, SEEK_END);
  data = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  if (data == NULL)
    return 0;
  if (memcmp(data, "#!", 2) == 0)
    r = is_script_dgsh_program(data, file_size);
  else
#if __APPLE__
    r = is_macho_dgsh_program(path);
#else
    r = is_elf_dgsh_program(data);
#endif
  munmap(data, file_size);
  return r;
}


#ifdef DGSH_COMPAT
int
main(int argc, char *argv[])
{
  if (argc != 2)
    errx(1, "usage: %s program", argv[0]);
  return is_dgsh_program(argv[1]) ? 0 : 2;
}
#endif
