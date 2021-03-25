#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "sec_run.h"

int get_instruction_dst(memory_table_line ** table,int current_index)
{
  return table[current_index]->label[1] - '0';
}

int sec_run(label_table_line *** label_table, int * label_table_size,memory_table_line *** data_table,int * data_table_size, memory_table_line*** instruction_table,int * instruction_table_size, func_node** hash_func_table, label_table_line*** entry_table, int * entry_table_size,int * entry_table_true_size,label_table_line*** extern_table, int * extern_table_size,int * extern_table_true_size, int ICF,char * file_name)
{
    int i;
    int j;
    int error_counter = 0;
	char ARE;
    int operand_num;
    int found_flag;
    int instration_dist;   /*if attribiute is relative, we need to fine the addrrese of the code line of the argument, which is 1 or 2 lines away*/
    word temp_word;
    /*shifting data addresses to it's place after code addresses*/
    for (i = 0; i < *data_table_size; i++)
    {
        (*data_table)[i]->address = ICF + i;
    }
    
    /*setting updated value for label table data and string addresses*/
    for (i = 0; i < *label_table_size; i++)
    {
        if ( (*label_table)[i]->attribute == Data || (*label_table)[i]->attribute == String)
        {
            for (j = 0; j < *data_table_size; j++)
            {
                if (!strcmp((*label_table)[i]->label, (*data_table)[j]->label))
                {
                    (*label_table)[i]->value = (*data_table)[j]->address; 
                    break;
                }          
            }
        }
    }
    /*setting updated value for label table entry addresses*/
    for (i = 0; i < *entry_table_size; i++)
    {
        found_flag = 0;
        for (j = 0; j < *label_table_size; j++)
        {
            if((!strcmp((*entry_table)[i]->label, (*label_table)[j]->label)))
                {
                    (*entry_table)[i]->value = (*label_table)[j]->value;
                    found_flag = 1;
                    break;
                }
        }
        if (!found_flag)
        {
            for (j = 0; j < *extern_table_size; j++)
            {
                if((!strcmp((*entry_table)[i]->label, (*extern_table)[j]->label)))
                    found_flag = 1;
            }
        }
        if (!found_flag)
        {            
            printf("ERROR in file %s\tin line %d:\tentry label %s has not been found in memory\n",file_name,(*label_table)[i]->value,(*entry_table)[i]->label);
            error_counter++;   
        }
    }     

    /*going throw each codeline in the instruction code to put accuret address values in arguments*/
    for(i = 0; i< *instruction_table_size; i++)
    {
        found_flag = 0;
        /*if ARE is undefine - this is an unknown argument*/
        if ((*instruction_table)[i]-> ARE  == 'A') continue;
        
        /*if machine code is undefine - this is an unknown argument*/
        /*if attribute is relative, the argument value is the diffrense between it's address to the data address*/
        if((*instruction_table)[i]->label[0] == '%')
        {
            ARE = 'R';
            for (j = 0; j < *label_table_size; j++)
            {
            /*identical label has been found in the label table*/
                if  (  (*label_table)[j]->attribute == Code && (!strcmp(&(*instruction_table)[i]->label[2], (*label_table)[j]->label)))
                {
                    found_flag = 1;
                    break;
                }
            }
            if(!found_flag)
            {
                printf("ERROR in file %s\tin line %d\tlabel \"%s\" is not found in data image\n",file_name,(*instruction_table)[i]->line_code_num,& (*instruction_table)[i]->label[2]);
                error_counter++;
                continue;
            }
            
            /*relative address is the distatnce between the instruction line and the label address*/
            instration_dist = get_instruction_dst(*instruction_table,i);
            operand_num = (*label_table)[j]->value - ((*instruction_table)[i - instration_dist]->address + 1);
            put_immediate_to_word(&temp_word, operand_num);
            (*instruction_table)[i]->ARE = ARE;
            (*instruction_table)[i]->machine_code = temp_word;
            continue;

        }
        
        /*attribiute is not relative*/
        found_flag = 0;
        /*search in exrern*/
        for (j = 0; j < *extern_table_size; j++)
        {
            /*identical label has been found in the label table*/
            if(!strcmp((*instruction_table)[i]->label, (*extern_table)[j]->label))
            {
                /*  
                    if label is seen for the first time in this iterration, we only change
                    it's address else we make another label in extern label - because this
                    extern lable is used in the program more than once.
                    attribute in thie table tells if this label has been seen more than one             */
                    
                if ((*extern_table)[j]->attribute == 0)
                    {
                    (*extern_table)[j]->value = (*instruction_table)[i]->address;
                    (*extern_table)[j]->attribute = 1;
                    }
                else
                    add_line_to_label_table(extern_table, (*instruction_table)[i]->address, (*instruction_table)[i]->label, 1, extern_table_size, extern_table_true_size);
                
                put_immediate_to_word(&temp_word, 0);
                (*instruction_table)[i]->ARE = 'E';
                (*instruction_table)[i]->machine_code = temp_word;
                found_flag = 1;
                break;
            }

        }

        if (found_flag) continue;
        for (j = 0; j < *label_table_size; j++)
        {
            /*identical label has been found in the label table*/
            if(!strcmp((*instruction_table)[i]->label, (*label_table)[j]->label))
            {
                put_immediate_to_word(&temp_word, (*label_table)[j]->value);
                (*instruction_table)[i]->ARE = 'R';
                (*instruction_table)[i]->machine_code = temp_word;
                found_flag = 1;
                break;
            }
            
        }

        if(!found_flag)
        {
            printf("ERROR in file %s\tin line %d:\tlabel %s is not found in label table\n",file_name,(*instruction_table)[i]->line_code_num,			  (*instruction_table)[i]->label);
            error_counter++;
        }
    }      

    /* updating all values of entry table */
    for(i = 0; i < *entry_table_size; i++)
    {   
        found_flag = 0;
        for(j = 0; j < *label_table_size; j++)
        {
            if(!strcmp((*entry_table)[i]->label, (*label_table)[j]->label))
            {
                (*entry_table)[i]->value = (*label_table)[j]->value;
                found_flag = 1;
                break;
            }
        }
        
        if(!found_flag)
        {
            printf("ERROR in file %s\tin line %d:\tentry label %s is not found in label table\n",file_name,(*entry_table)[i]->value,(*entry_table)[i]->label);
            error_counter++;  
        }
    }
	return error_counter;
}
