#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"

/*-----------------------a-single-word--------------------------*/

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

/* this method inserts a new line to the label table*/
void add_line_to_label_table(label_table_line *** current, int new_value, char new_label[], int new_attribute,int * label_table_size, int * label_table_true_size)
{
	label_table_line * temp_line;
	/*increasing table line*/
	label_table_line ** temp_table;
	/*if no label table - create one*/

	if (!*label_table_true_size) 
	{
		temp_table = malloc(sizeof(label_table_line *));
		temp_line = malloc(sizeof(label_table_line));
		(*label_table_true_size)++;
		if(!temp_table || !temp_line)
		{
			printf("FATAL ERROR: no space in disk\n");
			return;
		}	
	}
	else if(*label_table_size == *label_table_true_size)
	{
		temp_table = realloc(*current,sizeof(label_table_line *) * ((*label_table_true_size)+1));
		(*label_table_true_size)++;
		if(!temp_table)
			{
				printf("FATAL ERROR: no space in disk\n");
				return;
			}	
		/*making space for new line struct*/
		temp_line = malloc(sizeof(label_table_line));
	}
	else
	{
		temp_table = *current;
		temp_line = (*current)[*label_table_size];
	}
	
	/*insert value of new line*/
	temp_line -> value = new_value;
	strcpy(temp_line -> label,new_label);
	temp_line -> attribute = new_attribute;
	/*putting new line in table*/
	temp_table[(*label_table_size)] = temp_line;

	(*label_table_size)++;

    *current = temp_table;
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
}


/*--------------------------memory-table------------------------------*/

/*
note - this table should apper in the code two times-
one for instruction table (the code without the .data and .string)
and one for data table (only .data and .string)
*/


/* this method enter a new line to the memory table*/
void add_line_to_memory_table(memory_table_line *** current, int new_address, word new_machine_code ,char new_ARE,char new_label[], int * memory_table_size,int * memory_table_true_size, int new_line_code_num)
{
	memory_table_line * temp_line;
	/*increasing table line*/
	memory_table_line ** temp_table;
	/*if there is no label table - create one*/
	if (!*memory_table_true_size)
	{ 
		temp_table = malloc(sizeof(memory_table_line *));
		temp_line = malloc(sizeof(memory_table_line));
		(* memory_table_true_size)++;
		if(!temp_table || !temp_line)
		{
			printf("FATAL ERROR: no space in disk\n");
			return;
		}
	}
	else if (*memory_table_true_size == *memory_table_size)
	{
		temp_table = realloc(*current,sizeof(memory_table_line *) * ((*memory_table_true_size)+1));
		(*memory_table_true_size)++;

	if(!temp_table)
		{
			printf("FATAL ERROR: no space in disk\n");
			return;
		}	
		/*making space for new line struct*/
		temp_line = malloc(sizeof(memory_table_line));
	}
	else
	{
		temp_table = *current;
		temp_line = (*current)[*memory_table_size];
	}
	
	/*insert value of new line*/
	temp_line -> address = new_address;
  	temp_line -> machine_code = new_machine_code;
  	temp_line -> ARE = new_ARE;
	temp_line -> line_code_num = new_line_code_num;
	strcpy(temp_line -> label,new_label);
	
	/*putting new line in table*/
	temp_table[(*memory_table_size)] = temp_line;

	(*memory_table_size)++;

    *current = temp_table;
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
}
/*--------------------------hash-table----------------------------*/


int hash_function(char * input,int table_size)
{
    if (input[0] == '.') return 0;
	if (input[0] >= '0' && input[0] <= '9')
		return  (((input[0] - '0' + 1) * (input[0] - '0' + 1)) % table_size);
	return (((tolower(input[0]) - 'a') * (tolower(input[0]) - 'a')) % table_size);
}


void insert_to_hash_table(int key, char* buf,node** hash_table)
{
    node* new_ptr;
    if (!(new_ptr  = malloc(sizeof(node))))
    {
		printf("FATAL ERROR: no space in disk\n");
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
			printf("FATAL ERROR: no space in disk\n");
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
	int key = hash_function(buf,FUNCS_AMOUNT);
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

void erase_func_hash_table(func_node ** func_hash_table, int func_hash_table_size)
{
	int i;

	for(i = 0; i < func_hash_table_size; i++)
	{
		if(func_hash_table[i] != 0)
		{
			func_recurtion_erase(func_hash_table[i]->next);
			free(func_hash_table[i]);
		}
	}
	return;
}

void func_recurtion_erase(func_node * ptr)
{
	if (ptr->name != 0)
	{
	func_recurtion_erase(ptr->next);
	free(ptr);	
	}
	else return;
}

void erase_hash_table(node ** hash_table, int hash_table_size)
{
	int i;

	for(i = 0; i < hash_table_size; i++)
	{
		if(hash_table[i] != 0)
		{
			recurtion_erase(hash_table[i]->next);
			free(hash_table[i]);
			hash_table[i] = NULL;
		}
	}
	return;
}

void recurtion_erase(node * ptr)
{
	if (ptr->name != 0)
	{
	recurtion_erase(ptr->next);
	free(ptr);	
	}
	else return;
}
