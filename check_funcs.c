#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "check_funcs.h"

int is_legal_reg(char *reg_name)
{
	return (reg_name[0] == 'r' && (reg_name[1]<= '9' && reg_name[1] >= '0'));
}

int is_data(int label_flag,char ** slot_array)
{
	if (label_flag) return (!strcmp(slot_array[1], ".data"));
	return (!strcmp(slot_array[0], ".data"));  
}

int is_string(int label_flag,char ** slot_array)
{
	if (label_flag) return (!strcmp(slot_array[1], ".string"));
	return (!strcmp(slot_array[0], ".string"));  
}

int is_valid_data(char ** slot_array,int label_flag, int argument_num)
{
  	int i;
    for (i = label_flag + 1; i <= argument_num; i++)
    {
		if (!is_valid_imd(slot_array[i]))
			return 0;
    }
	return 1;
}

int is_valid_string(char ** slot_array,int label_flag, int argument_num)
{
	int i;
    for (i = label_flag ? 2 : 1 ;i <= argument_num; i++)
    {
		if (*slot_array[i] > 'z' || *slot_array[i] < 'a') 
			return 0;
    }
	return 1;
}

int is_entry(char ** slot_array,int label_flag, int argument_num)
{
	int i = label_flag?1:0;
	return !(strcmp(slot_array[i],".entry"));
}

int is_extern(char ** slot_array,int label_flag, int argument_num)
{
	int i = label_flag?1:0;
	return !(strcmp(slot_array[i],".extern"));
}

/*check if a given lable is vaild, if so enter it to hash_saved_words and return 1, else return 0*/
int is_valid_label(char * label, node** hash_saved_words, node** hash_lable_names)
{
	if( !is_in_hash_table(hash_saved_words,26,label)	 &&
		!is_in_hash_table(hash_lable_names,26,label)	)
	{
		insert_to_hash_table(hash_function(label,26), label,hash_lable_names);
		return 1;
	}
	return 0;
}

/*checks if a given char is a valid imidiant number*/
int is_valid_imd(char * imd_val)
{
	int i = 1;
	if ((imd_val[0] < '0' || imd_val[0] > '9')	 &&
		(imd_val[0] != '-' && imd_val[0] != '+')) 
			return 0;
		while ( imd_val[i] != '\0' )
	{
		if (imd_val[i] > '9' || imd_val[i] < '0')
			return 0;
		i++;
	}
	return 1;
}