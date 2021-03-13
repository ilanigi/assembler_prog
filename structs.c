#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"

/*-----------------------a-single-word--------------------------*/

void print_word(word word_toprint)
{
	int i;
	int mask;
	short int temp;
	/*if all parameters are 0, print nothing */
	if (!(word_toprint.opcode || word_toprint.funct || word_toprint.source || word_toprint.destenation))
	{
		printf("000000000000");
		return;
	}
	temp = word_toprint.opcode;
	for (i = 0, mask = 8; mask; mask >>= 1, i++)
	{
		printf("%d",(word_toprint.opcode & mask) >> (3-i));
	}
	
	for (i = 0, mask = 8; mask; mask >>= 1, i++)
	{
		printf("%d",(word_toprint.funct & mask) >> (3-i) );
	}
	
	for (i = 0, mask = 2; mask; mask >>= 1, i++)
	{
		printf("%d",(word_toprint.source & mask) >> (1 - i));
	}

	for (i = 0, mask = 2; mask; mask >>= 1, i++)
	{
		printf("%d",(word_toprint.destenation & mask) >> (1 - i));
	}
}	
/*enter an imidate nambuer into a word */
void put_immediate_to_word(word * current,int num)
{
	int mask = 3;
	current->destenation = mask & num;
	mask <<= 2;
	current->source = (mask & num)>> 2;
	mask = 0xf0;
	current->funct = (mask & num)>> 4;
	mask <<= 4;
	current->opcode = (mask & num)>> 8;
	return;
}

/*-----------------------label-table--------------------------*/

/* this method inserts a new line to the lable table*/
void add_line_to_label_table(label_table_line *** current, int new_value, char new_label[], char new_attribute[],int * label_table_size)
{
	label_table_line * temp_line;
	/*increasing table line*/
	label_table_line ** temp_table;
	/*if no lable table - create one*/
	if (!*label_table_size) 
		temp_table = malloc(sizeof(label_table_line *));
	
	else
		temp_table = realloc(*current,sizeof(label_table_line *) * ((*label_table_size)+1));

	if(!temp_table)
		{
			printf("FATAL ERROR: no space in disk");
			return;
		}	
	
	/*making space for new line struct*/
	temp_line = malloc(sizeof(label_table_line));
	
	/*insert value of new line*/
	temp_line -> value = new_value;
	strcpy(temp_line -> label,new_label);
	strcpy(temp_line -> attribute,new_attribute);
	/*putting new line in table*/
	temp_table[(*label_table_size)] = temp_line;

	(*label_table_size)++;

    *current = temp_table;
	return;
}

void print_label_table(label_table_line ** table,int label_table_size)
{
    int i;
	if (!label_table_size)
	{
		printf("lable table is empty\n");	
		return;	
	}
	printf("symbol\tvalue\tattribute\n");
	for (i = 0; i < label_table_size; i++)
	{
		printf("%s\t%d\t\t%s\n",table[i]->label,table[i]->value,table[i]->attribute);
	}
	return;    
}

void erase_label_table(label_table_line ** table,int * label_table_size)
{
	int i;
	for (i = 0; i < *label_table_size; i++)
	{
		free(table[i]);
	}
	free(table);
	*label_table_size = 0;
	printf("lable table erased sucsesfully\n");
}


/*--------------------------memory-table------------------------------*/

/*
note - this table should apper in the code two times-
one for instruction table (the code without the .data and .string)
and one for data table (only .data and .string)
*/


/* this method enter a new line to the memory table*/
void add_line_to_memory_table(memory_table_line *** current_table, int new_address, word new_machine_code ,char new_ARE,char new_label[], int * memory_table_size, int new_line_code_num)
{
	memory_table_line * temp_line;
	/*increasing table line*/
	memory_table_line ** temp_table;
	/*if there is no lable table - create one*/
	if (!*memory_table_size) 
		temp_table = malloc(sizeof(memory_table_line *));
	
	else
		temp_table = realloc(*current_table,sizeof(memory_table_line *) * ((*memory_table_size)+1));

	if(!temp_table)
		{
			printf("FATAL ERROR: no space in disk");
			return;
		}	
	
	/*making space for new line struct*/
	temp_line = malloc(sizeof(memory_table_line));
	
	/*insert value of new line*/
	temp_line -> address = new_address;
  	temp_line -> machine_code = new_machine_code;
  	temp_line -> ARE = new_ARE;
	temp_line -> line_code_num = new_line_code_num;
	strcpy(temp_line -> label,new_label);
	
	/*putting new line in table*/
	temp_table[(*memory_table_size)] = temp_line;

	(*memory_table_size)++;

    *current_table = temp_table;
	return;
}
void print_memory_table(memory_table_line ** table,int memory_table_size)
{
    int i;

	if (!memory_table_size)
	{
		printf("memory table is empty\n");	
		return;	
	}
	/*first line is headers for table*/
	printf("address\tmachine code\t\"ARE\"\tlabel\n");
	for (i = 0; i < memory_table_size; i++)
	{
		/*add zero for leading zero, if necessary*/
		if (table[i]->address <= 999)
			printf("0%d\t",table[i]->address);
		else
			printf("%d\t",table[i]->address);
	
		print_word(table[i]->machine_code);
	
		printf("\t");
		
		printf("%c\t",table[i]->ARE);
	
		printf("%s\n",table[i]->label);
	}
	return;    
}

/*free all memory of memory table - at end of the second run*/

void erase_memory_table(memory_table_line ** table,int * memory_table_size)
{
	int i;
	for (i = 0; i < *memory_table_size; i++)
	{
		/*free each struct in table*/
		free(table[i]);
	}
	/*free table base array*/
	free(table);
	*memory_table_size = 0;
	printf("memory table erased sucsesfully\n");
}
/*--------------------------hash-table----------------------------*/


int hash_function(char * input,int table_size)
{
    if (input[0] == '.') return 0;
	return ((tolower(input[0]) - 'a') % table_size);
}


void insert_to_hash_table(int key, char* buf,node** hash_table)
{
    node* new_ptr;
    if (!(new_ptr  = malloc(sizeof(node))))
    {
		printf("no place in memory\n");
        return;
    }
    strcpy(new_ptr->name, buf);
    new_ptr->next = 0;

    if (hash_table[key] == 0) hash_table[key] = new_ptr;

    else
    {
        node* node_ptr = hash_table[key];
        while (1)
        {
            if (node_ptr->next == 0)
            {
                node_ptr->next = new_ptr;
                break;
            }
            node_ptr = node_ptr->next;
        }
    }
}

void insert_to_func_hash_table(int key, char * buf, short int num, func_node** hash_table)
{
    func_node* new_ptr;
    if (!(new_ptr  = malloc(sizeof(func_node))))
    {
		printf("no place in memory\n");
        return;
    }

    strcpy(new_ptr->name, buf);
	new_ptr->num = num;
    new_ptr->next = 0;

    if (hash_table[key] == 0) hash_table[key] = new_ptr;

    else
    {
        func_node* node_ptr = hash_table[key];
        while (1)
        {
            if (node_ptr->next == 0)
            {
                node_ptr->next = new_ptr;
                break;
            }
            node_ptr = node_ptr->next;
        }
    }
}


/*return 1 if buf in size, else 0*/
int is_in_hash_table(node ** hash_table,int table_size, char* buf)
{
	int key = hash_function(buf,table_size);
	if (hash_table[key] == NULL) return 0;
	else
	{
		node * node_ptr = hash_table[key];
		while (1)
		{
			if (!strcmp(node_ptr->name,buf))
			{
				return 1;
			}
			if (node_ptr->next == 0) return 0;
			else node_ptr = node_ptr->next;
		}
	}
}

/*return func num if func valid, else return -1*/
int is_valid_func(func_node ** hash_table, char* buf)
{
	int key = hash_function(buf,16);
	if (hash_table[key] == NULL) return 0;
	else
	{
		func_node * node_ptr = hash_table[key];
		while (1)
		{
			if (!strcmp(node_ptr->name,buf))
			{
				return node_ptr->num;
			}
			if (node_ptr->next == NULL) return -1;
			else node_ptr = node_ptr->next;
		}
	}
}