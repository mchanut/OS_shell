#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

int pid_counter = 1;

int generatePID(){
    return pid_counter++;
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(int start, int end){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = generatePID();
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    newPCB->job_length_score = 1+end-start;
    newPCB->priority = false;
    newPCB->page_table;
    newPCB->table_counter = 0;
    newPCB->line_counter = 1;
    newPCB->script;
    newPCB->page_handled = false;
    return newPCB;
}

PCB* makePCB_with_table(char* script){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = generatePID();
    newPCB->PC = 0;
    newPCB->start  = 0;
    newPCB->end = 0;
    newPCB->job_length_score = 0;
    newPCB->priority = false;
    newPCB->page_table;
    newPCB->table_counter = 0;
    newPCB->line_counter = 1;
    strcpy(newPCB->script, script);
    newPCB->page_handled = false;
    return newPCB;
}