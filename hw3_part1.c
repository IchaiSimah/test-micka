//#include <stdio.h>
//#include <stdarg.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <syscall.h>
//#include <sys/ptrace.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <sys/reg.h>
//#include <sys/user.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>
//#include <stdbool.h>
//
//#include "elf64.h"
//
//#define	ET_NONE	0	//No file type
//#define	ET_REL	1	//Relocatable file
//#define	ET_EXEC	2	//Executable file
//#define	ET_DYN	3	//Shared object file
//#define	ET_CORE	4	//Core file
//
//#define SHT_SYMTAB 3
//#define STB_GLOBAL 32
//
//
///* symbol_name		- The symbol (maybe function) we need to search for.
// * exe_file_name	- The file where we search the symbol in.
// * error_val		- If  1: A global symbol was found, and defined in the given executable.
// * 			- If -1: Symbol not found.
// *			- If -2: Only a local symbol was found.
// * 			- If -3: File is not an executable.
// * 			- If -4: The symbol was found, it is global, but it is not defined in the executable.
// * return value		- The address which the symbol_name will be loaded to, if the symbol was found and is global.
// */
//
//
//
//unsigned long find_symbol(char* symbol_name, char* exe_file_name, int* error_val) {
//    if (!symbol_name || !exe_file_name){
//        *error_val = -3;
//        return 0;
//    }
//
//    FILE* file = fopen(exe_file_name, "r");
//    if (!file) {
//        *error_val = -3;
//        return 0;
//    }
//
//    // Read ELF header
//    Elf64_Ehdr* elf_header;
//    fread(elf_header, sizeof(Elf64_Ehdr),1, file);
//
//    // Check if file is executable
//    if (elf_header->e_type != ET_EXEC) {
//        *error_val = -3;
//        fclose(file);
//        return 0;
//    }
//
//    //read section header
//    Elf64_Shdr* section_header = malloc(elf_header->e_shentsize * elf_header->e_shnum);
//    fseek(file, elf_header->e_shoff, SEEK_SET);
//    fread(section_header, elf_header->e_shentsize, elf_header->e_shnum, file);
//
//    // Read symbol table information
//    Elf64_Shdr* symbol_table_header= malloc(elf_header->e_shstrndx);
//    fseek(file, elf_header->e_shoff + elf_header->e_shstrndx * sizeof(Elf64_Shdr), SEEK_SET);
//    fread(&symbol_table_header, sizeof(Elf64_Shdr), 1, file);
//
//    // Read symbol table string names
//    char *symbol_table_string_names = malloc(symbol_table_header->sh_size);
//    fseek(file, symbol_table_header->sh_offset, SEEK_SET);
//    fread(symbol_table_string_names, symbol_table_header->sh_size, 1, file);
//
//
//
//    // Find symbol table section
//    for (int i = 0; i < elf_header->e_shnum; i++) {
//        Elf64_Shdr section_header;
//        fseek(file, elf_header->e_shoff, SEEK_SET);
//        fread(&section_header, sizeof(Elf64_Shdr), 1, file);
//
//        if (section_header.sh_type == SHT_SYMTAB) {
//            // Read symbol table
//            Elf64_Sym *symbol_table = malloc(section_header.sh_size);
//            fseek(file, section_header.sh_offset, SEEK_SET);
//            fread(symbol_table, section_header.sh_size, 1, file);
//
//            // Find symbol
//            for (int j = 0; j < section_header.sh_size / sizeof(Elf64_Sym); j++) {
//                if (strcmp(symbol_name, symbol_table_string_names + symbol_table[j].st_name) == 0) {
//                    // Symbol found
//                     if (ELF64_ST_BIND(symbol_table[j].st_info) == STB_GLOBAL) {
//                         // Symbol is global
//                         if (symbol_table[j].st_shndx == SHN_UNDEF) {
//                             // Symbol is global, but not defined in the executable
//                             *error_val = -4;
//                             fclose(file);
//                             free(symbol_table);
//                             free(symbol_table_string_names);
//                             return 0;
//                         } else {
//                             // Symbol is global and defined in the executable
//                             *error_val = 1;
//                             unsigned long symbol_address = symbol_table[j].st_value;
//                             fclose(file);
//                             free(symbol_table);
//                             free(symbol_table_string_names);
//                             return symbol_address;
//                         }
//                     }else {
//                        // Symbol is local
//                        *error_val = -2;
//                        fclose(file);
//                        free(symbol_table);
//                        free(symbol_table_string_names);
//                        return 0;
//                    }
//                }
//            }
//            // Symbol not found
//            *error_val = -1;
//            fclose(file);
//            free(symbol_table);
//            free(symbol_table_string_names);
//            return 0;
//        }
//    }
//}
//
//
//int main(int argc, char *const argv[]) {
//    int err = 0;
//    unsigned long addr = find_symbol(argv[1], argv[2], &err);
//
//    if (err > 0)
//        printf("%s will be loaded to 0x%lx\n", argv[1], addr);
//    else if (err == -2)
//        printf("%s is not a global symbol! :(\n", argv[1]);
//    else if (err == -1)
//        printf("%s not found!\n", argv[1]);
//    else if (err == -3)
//        printf("%s not an executable! :(\n", argv[2]);
//    else if (err == -4)
//        printf("%s is a global symbol, but will come from a shared library\n", argv[1]);
//    return 0;
//}
//


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "elf64.h"

#define ET_NONE 0 // No file type
#define ET_REL 1  // Relocatable file
#define ET_EXEC 2 // Executable file
#define ET_DYN 3  // Shared object file
#define ET_CORE 4 // Core file

/* symbol_name		- The symbol (maybe function) we need to search for.
 * exe_file_name	- The file where we search the symbol in.
 * error_val		- If  1: A global symbol was found, and defined in the given executable.
 * 			- If -1: Symbol not found.
 *			- If -2: Only a local symbol was found.
 * 			- If -3: File is not an executable.
 * 			- If -4: The symbol was found, it is global, but it is not defined in the executable.
 * return value		- The address which the symbol_name will be loaded to, if the symbol was found and is global.
 */

#define STB_GLOBAL 1



int getIndex(Elf64_Shdr *section_header, Elf64_Ehdr *elf_header, char *section_header_string_table, char *section_name){
    for (int i = 0; i < elf_header->e_shnum; i++){
        if (strcmp(section_header_string_table + section_header[i].sh_name, section_name) == 0){
            return i;
        }
    }
    return -1;
}



Elf64_Sym *getSymbol(Elf64_Shdr *symbol_table_header, char *string_table, char *symbol_table, char *input){
    int num_of_symbols = (symbol_table_header->sh_size / symbol_table_header->sh_entsize);
    Elf64_Sym *to_return = NULL;
    for (int i = 0; i < num_of_symbols; i++){
        Elf64_Sym *symbol = (Elf64_Sym *)(symbol_table + i * symbol_table_header->sh_entsize);
        char *symbol_name = string_table + symbol->st_name;
        if (strcmp(symbol_name, input) == 0){
            if (ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL){
                return symbol;
            }
            to_return = symbol;
        }
    }
    return to_return;
}



unsigned long find_symbol(char *symbol_name, char *exe_file_name, int *error_val){
    if (!symbol_name || !exe_file_name){
        *error_val = -1;
        return 0;
    }
    FILE *file = fopen(exe_file_name, "rb");
    if (file == NULL){
        *error_val = -3;
        return 0;
    }

    Elf64_Ehdr elf_header;
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, file);
    if (elf_header.e_type != ET_EXEC){
        fclose(file);
        *error_val = -3;
        return 0;
    }

    //read section header
    Elf64_Shdr section_header[elf_header.e_shentsize * elf_header.e_shnum];
    fseek(file, elf_header.e_shoff, SEEK_SET);
    fread(section_header, elf_header.e_shentsize, elf_header.e_shnum, file);

    // read string table header
    Elf64_Shdr header_section_header_string_table = section_header[elf_header.e_shstrndx];
    char *section_header_string_table = (char *)malloc(header_section_header_string_table.sh_size);
    fseek(file, header_section_header_string_table.sh_offset, SEEK_SET);
    fread(section_header_string_table, header_section_header_string_table.sh_size, 1, file);

    //get index of symbol
    int symtab_index = getIndex(section_header, &elf_header, section_header_string_table, ".symtab");
    Elf64_Shdr symbol_table_header = section_header[symtab_index];
    char *symbol_table = (char *)malloc(symbol_table_header.sh_size);
    fseek(file, symbol_table_header.sh_offset, SEEK_SET);
    fread(symbol_table, symbol_table_header.sh_size, 1, file);

    //get index of string tab
    int string_tab_index = getIndex(section_header, &elf_header, section_header_string_table, ".strtab");
    Elf64_Shdr string_table_header = section_header[string_tab_index];
    char *string_table = (char *)malloc(string_table_header.sh_size);
    fseek(file, string_table_header.sh_offset, SEEK_SET);
    fread(string_table, string_table_header.sh_size, 1, file);

    //check symbol status
    Elf64_Sym *symbol = getSymbol(&symbol_table_header, string_table, symbol_table, symbol_name);
    if (!symbol){
        *error_val = -1;
        free(section_header_string_table);
        free(symbol_table);
        free(string_table);
        fclose(file);
        return 0;
    }
    if (ELF64_ST_BIND(symbol->st_info) != STB_GLOBAL){
        *error_val = -2;
        free(section_header_string_table);
        free(symbol_table);
        free(string_table);
        fclose(file);
        return 0;
    }
    if (symbol->st_shndx == SHN_UNDEF){
        *error_val = -4;
        free(section_header_string_table);
        free(symbol_table);
        free(string_table);
        fclose(file);
        return 0;
    }
    *error_val = 1;
    free(section_header_string_table);
    free(symbol_table);
    free(string_table);
    fclose(file);
    return symbol->st_value;
}



int main(int argc, char *const argv[])
{
    int err = 0;

    unsigned long addr = find_symbol(argv[1], argv[2], &err);

    if (addr > 0)
        printf("%s will be loaded to 0x%lx\n", argv[1], addr);
    else if (err == -2)
        printf("%s is not a global symbol! :(\n", argv[1]);
    else if (err == -1)
        printf("%s not found!\n", argv[1]);
    else if (err == -3)
        printf("%s not an executable! :(\n", argv[2]);
    else if (err == -4)
        printf("%s is a global symbol, but will come from a shared library\n", argv[1]);
    return 0;
}
