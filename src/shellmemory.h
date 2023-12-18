#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H
#include "pcb.h"
void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int load_file(FILE* fp, int* pStart, int* pEnd, char* fileID, PCB * pcb);
char * mem_get_value_at_line(int index, int line);
void mem_free_lines_between(int start, int end);
void printShellMemory();
int check_space_mem();
void load_missing_page(PCB* pcb, int index);
void evict_page();
#endif