#include "structs.h"
#include "init.h"
#include <stdio.h>


void init_hash(func_node** hash_func_table, node** hash_saved_words)
{
	char * saved_words[] = {
							"r1","r2","r3","r4","r5","r6","r7","r8",
							"mov","cmp","add","sub","lea","clr","not",
							"inc","dec","jmp","bne","jsr","red","prn",
							"rts","stop",
							".data", ".entry",".string",".extern",
							"data", "entry","string","extern"
							};
						
	char * all_func[] = {
						"mov","cmp","add","sub","lea","clr","not",
						"inc","dec","jmp","bne","jsr","red","prn",
						"rts","stop"
						};

	build_hash_table(saved_words, hash_saved_words, HASH_TABLE_SIZE);
	build_func_hash_table(all_func, hash_func_table, FUNCS_AMOUNT);
}

void build_hash_table(char ** input_table,node ** output_table, int table_size)
{
	int i;
	for (i = 0; i < table_size;i++)
	{
		insert_to_hash_table(hash_function(input_table[i],table_size),input_table[i],output_table);	
	}
}

void build_func_hash_table(char ** input_table,func_node ** output_table, int table_size)
{
	int i;
	for (i = 0; i < table_size;i++)
	{
		insert_to_func_hash_table(hash_function(input_table[i],table_size),input_table[i],i,output_table);
	}
}

