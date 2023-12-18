#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include "pcb.h"

int SHELL_MEM_LENGTH = VAR_STORE_SIZE;
int FRAME_MEM_LENGTH = FRAME_STORE_SIZE/3;


struct var_memory_struct{
	char *var;
	char *value;
};

struct frame_memory_struct{
	char *line1;
	char *line2;
	char *line3;
};

struct LRU_frame{
	int frame_index;
	int counter;
	int access;
};

struct LRU_frame lru_frame[50];


struct frame_memory_struct framememory[FRAME_STORE_SIZE];

struct var_memory_struct shellmemory[VAR_STORE_SIZE];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){
	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
	for (i=0; i<FRAME_MEM_LENGTH; i++){		
		framememory[i].line1 = "none";
		framememory[i].line2 = "none";
		framememory[i].line3 = "none";
	}
	for (i=0; i<50; i++){		
		lru_frame[i].frame_index = 0;
		lru_frame[i].counter = 0;
		lru_frame[i].access = 0;
	}

}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}


void printFrameMemory(){
	int count_empty = 0;
	for (int i = 0; i < FRAME_MEM_LENGTH; i++){
		if(strcmp(framememory[i].line1,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\t key2 : %s\t\t key3 : %s\n", i, framememory[i].line1, framememory[i].line2, framememory[i].line3);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", FRAME_MEM_LENGTH, FRAME_MEM_LENGTH-count_empty, count_empty);
}


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */

int load_file(FILE* fp, int* pStart, int* pEnd, char* filename, PCB* pcb)
{
	char *line;
    size_t i = 0;
    int error_code = 0;
	bool hasSpaceLeft = false;
	bool flag = true;
	i=0;
	size_t candidate;
	while(flag){
		flag = false;
		for (i; i < FRAME_MEM_LENGTH; i++){
			if(strcmp(framememory[i].line1,"none") == 0){
				*pStart = (int)i;
				hasSpaceLeft = true;
				break;
			}
		}
		candidate = i;
		for(i; i < FRAME_MEM_LENGTH; i++){
			if(strcmp(framememory[i].line1,"none") != 0){
				flag = true;
				break;
			}
		}
	}
	i = candidate;
	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}

    int pcb_page_counter = 0;
	size_t size = i;
    for (size_t j = i; j < FRAME_MEM_LENGTH; j++){
        if((feof(fp)) || (pcb_page_counter == 2))
        {
            *pEnd = (int)size-1;
            break;
        }else{
			pcb->page_table[pcb_page_counter] = j;
			pcb_page_counter++;
			
            fgets(line, 999, fp);
			size++;
            framememory[j].line1 = strndup(line, strlen(line));
			

        }
		if(feof(fp))
        {
            *pEnd = (int)size-1;
			framememory[j].line2 = "FRAME_COMPLETION";
			framememory[j].line3 = "FRAME_COMPLETION";
            break;
        }else{
			
            fgets(line, 999, fp);
			size++;
			framememory[j].line2 = strndup(line, strlen(line));
			
        }
		if(feof(fp))
        {
            *pEnd = (int)size-1;
			framememory[j].line3 = "FRAME_COMPLETION";
            break;
        }else{
			
            fgets(line, 999, fp);
			size++;
			framememory[j].line3 = strndup(line, strlen(line));
		
        }
    }

	//no space left to load the entire file into shell memory
	/*if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= FRAME_MEM_LENGTH; i ++){
			framememory[j].line1 = "none";
			framememory[j].line2 = "none";
			framememory[j].line3 = "none";
    	}
		return error_code;
	}*/
	char c;
	if(!feof(fp)){
		for (c = getc(fp); c != EOF; c = getc(fp)) {
			if (c == '\n') { // Increment count if this character is newline
            	size++;
			}
		}
		*pEnd = (int)size;
	}
	//printFrameMemory();
    return error_code;
	
}



char * mem_get_value_at_line(int index, int line){ // change that for A3
	if(index<0 || index > FRAME_MEM_LENGTH) return NULL; 
	if (line == 1) {
		lru_frame[lru_frame[0].counter++].frame_index = index;
		return framememory[index].line1;
	}
	if (line == 2) return framememory[index].line2;
	if (line == 3) return framememory[index].line3;
	return NULL;
}

void mem_free_lines_between(int start, int end){
	int i = start;
	int j = start;
	while(i<=end) {
		if(framememory[j].line1 != NULL){
			free(framememory[j].line1);
		}	
		if((framememory[j].line2 != NULL) && (strcmp(framememory[j].line2,"FRAME_COMPLETION")!=0)){
			free(framememory[j].line2);
		}	
		if((framememory[j].line3 != NULL) && (strcmp(framememory[j].line3,"FRAME_COMPLETION")!=0)){
			free(framememory[j].line3);
		}	
		framememory[j].line1 = "none";
		framememory[j].line2 = "none";
		framememory[j].line3 = "none";
		i+=3;
		j+=1;
	}
}

// works
int check_space_mem() {
	for (int i = 0; i < FRAME_MEM_LENGTH; i++){
			if(strcmp(framememory[i].line1,"none") == 0){
				return i;
			}
	}
	return -1;
}

void load_missing_page(PCB* pcb, int index) {
	char *line;
	char* script = (pcb->script);
	
	FILE* fp = fopen(script, "rt");
	
	int count = (pcb->table_counter)*3;
	int count2 = 0;
	while(count2 != count) {
		line = calloc(1, 1000);
		fgets(line, 999, fp);
		count2++;
	}

	line = calloc(1, 1000);

    fgets(line, 999, fp);
	framememory[index].line1 = strndup(line, strlen(line));
	if(feof(fp))
    {     
		framememory[index].line2 = "FRAME_COMPLETION";
		framememory[index].line3 = "FRAME_COMPLETION";
    }else{
        fgets(line, 999, fp);
		framememory[index].line2 = strndup(line, strlen(line));
    }
	if(feof(fp))
    {
		framememory[index].line3 = "FRAME_COMPLETION";
    }else{
        fgets(line, 999, fp);
		framememory[index].line3 = strndup(line, strlen(line));
    }
	pcb->page_table[pcb->table_counter] = index;
	//printFrameMemory();
	fclose(fp);
}

void evict_page(){
	int i = lru_frame[lru_frame[0].access++].frame_index;
	printf("Page fault! Victim page contents:\n\n%s%s%s\nEnd of victim page contents.\n", framememory[i].line1, framememory[i].line2, framememory[i].line3);
	free(framememory[i].line1);
	free(framememory[i].line2);
	free(framememory[i].line3);
	framememory[i].line1 = "none";
	framememory[i].line2 = "none";
	framememory[i].line3 = "none";
}