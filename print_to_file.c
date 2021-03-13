#include <stdio.h>

#include <string.h>

#include "structs.h"

#include "print_to_file.h"



int print_to_files(char * current_file_name, memory_table_line ** data_table, int data_table_size, memory_table_line ** instruction_table, int instruction_table_size,label_table_line ** label_table, int label_table_size, label_table_line ** extern_tabel, int extern_tabel_size, int ICF, int DCF,int has_entry)
{
    int i;
    FILE * file_ptr;
    char file_name[80];
    strcpy(file_name,current_file_name);
    strcat(file_name,".ob");
    printf("%s\n", file_name);
    if (!(file_ptr = fopen(file_name,"w+")))
    {
        printf("memory error, can't open new file\n");
        return 1;
    }

    print_table_to_file(instruction_table, instruction_table_size, file_ptr, ICF, DCF);
    print_table_to_file(data_table, data_table_size, file_ptr, 0, 0);
    fclose(file_ptr);

    if (extern_tabel_size)
    {
        strcpy(file_name,current_file_name);
        strcat(file_name,".ext");
        if (!(file_ptr = fopen(file_name,"w")))
        {
            printf("memory error, can't open new file\n");
            return 1;
        }
        print_to_extern(extern_tabel,extern_tabel_size,file_ptr);
        fclose(file_ptr);
    }

    if (has_entry)
    {
        strcpy(file_name,current_file_name);
        strcat(file_name,".ent");
        if (!(file_ptr = fopen(file_name,"w")))
        {
            printf("memory error, can't open new file\n");
            return 1;
        }
        print_to_entery(label_table, label_table_size, file_ptr);
        fclose(file_ptr);
    }    
    return 0;
    
}
void print_table_to_file(memory_table_line** table,int table_size,FILE * file_ptr, int ICF, int DCF)
{
    int i;
    if (ICF)
    {
        fprintf(file_ptr,"\t%d %d\n", ICF - 100, DCF);
    }
    
	for (i = 0; i < table_size; i++)
	{
		/*add zero for leading zero, if necessary*/
		if (table[i]->address <= 999)
			fprintf(file_ptr,"0%d ",table[i]->address);
        else
			fprintf(file_ptr,"%d ",table[i]->address);  
        print_word_to_file(table[i]->machine_code, file_ptr);
        fprintf(file_ptr," ");
		fprintf(file_ptr,"%c\n",table[i]->ARE);
	}
	return;  
}

void print_word_to_file(word word_toprint, FILE * file_ptr)
{
	int i;
    int temp;
	int mask = 15;
    
    if ((word_toprint.opcode & mask) >= 10)
        fprintf(file_ptr,"%c",(word_toprint.opcode & mask) + 'A' - 10);
    else
        fprintf(file_ptr,"%d",(word_toprint.opcode & mask));
    
    if ((word_toprint.funct & mask) >= 10)
        fprintf(file_ptr,"%c",(word_toprint.funct & mask) + 'A' - 10);
    else
        fprintf(file_ptr,"%d",(word_toprint.funct & mask));

    mask = 3;
    temp = (word_toprint.destenation & mask) + ((word_toprint.source & mask) << 2);

    if (temp >= 10)
        fprintf(file_ptr,"%c",temp + 'A' - 10);
    else
        fprintf(file_ptr,"%d",temp);
    
    return;

}

void print_to_extern(label_table_line ** extern_table, int extern_table_size,FILE * file_ptr)
{  
    int i;
    for(i = 0; i < extern_table_size; i++)
    {
        fprintf(file_ptr,"%s ",extern_table[i]->label);
        /*add zero for leading zero, if necessary*/
        if (extern_table[i]->value <= 999)
            fprintf(file_ptr,"0%d\n",extern_table[i]->value);
        else
            fprintf(file_ptr,"%d\n",extern_table[i]->value);
    }
    return;
}

void print_to_entery(label_table_line ** label_table, int label_table_size, FILE * file_ptr)
{
    int i;
    for(i = 0; i < label_table_size; i++)
    {
        if(!strcmp(label_table[i]->attribute,".entry"))
        {
            fprintf(file_ptr,"%s ",label_table[i]->label);
            if (label_table[i]->value <= 999)
			    fprintf(file_ptr,"0%d\n",label_table[i]->value);
            else
			    fprintf(file_ptr,"%d\n",label_table[i]->value);  
        }
    }
    return;
}