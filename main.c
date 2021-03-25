#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "first_run.h"
#include "init.h"
#include "sec_run.h"
#include "print_to_file.h"
#include "clean.h"

int main( int argc, char *argv[])
{
	/*init hash tabels*/
	func_node* hash_func_table[FUNCS_AMOUNT] = {0};
	node* hash_saved_words[HASH_TABLE_SIZE] = {0};
	/*init tables*/
	label_table_line ** label_table = {0};
	label_table_line ** entry_table = {0};
	memory_table_line** instruction_table = {0};
	memory_table_line** data_table = {0};
	label_table_line** extern_table = {0};
	int label_table_size = 0;
	int data_table_size = 0;
	int instruction_table_size = 0;
	int entry_table_size = 0;
	int extern_table_size = 0;

	/*
		To make memory allocation as least as possible,
		each table has a size - the his size in that file iteration,
		and a TRUE size - which is big or even to the regular size
		this is the real size of the table.
	*/
	int label_table_true_size = 0;
	int data_table_true_size = 0;
	int instruction_table_true_size = 0;
	int extern_table_true_size = 0;
	int entry_table_true_size = 0;

	int i = 1;
	int ICF;
	int DCF;
	FILE * fptr;
	int error_counter;
	char file_name[MAX_FILE_NAME_SIZE];
	
	/*init hash tables*/
	init_hash(hash_func_table, hash_saved_words);
	
	for (i = 1; i < argc; i++)	
	{

		/*open file to fptr*/
		strcpy(file_name,argv[i]);
		strcat(file_name,".as");
		if (!(fptr = fopen(file_name,"r")))
		{
			printf("ERROR:\tfile %s is not exsist\n",argv[i]);
			continue;
		}
		printf("starting process to file %s\n",argv[i]);
		error_counter = first_run(&label_table, &label_table_size, &data_table, &data_table_size, &instruction_table, &instruction_table_size, hash_func_table, hash_saved_words, &label_table_true_size,  &data_table_true_size, &instruction_table_true_size, &entry_table,&entry_table_size, &entry_table_true_size, &extern_table, &extern_table_size, &extern_table_true_size, fptr, &ICF, &DCF, argv[i]);
		fclose(fptr);
		error_counter += sec_run(&label_table, &label_table_size, &data_table, &data_table_size, &instruction_table, &instruction_table_size, hash_func_table,&entry_table, &entry_table_size, &entry_table_true_size, &extern_table,&extern_table_size, &extern_table_true_size,ICF,argv[i]);
		
		if(!error_counter)
		{
			printf("assembling process ended successfully. writing to files.\n");
			if (print_to_files(argv[i],data_table, data_table_size, instruction_table, instruction_table_size, label_table, label_table_size, extern_table,extern_table_size,entry_table,entry_table_size, ICF, DCF))
				printf("an error has occored while writing to output files in file %s\n",argv[i]);	
		}
		else
		{
			printf("file %s finished with %d errors and will not write to files\n",argv[i],error_counter);	
		}
		
	printf("file %s is done \n",argv[i]);		
	semi_clean(hash_saved_words,&label_table_size,&data_table_size,&instruction_table_size,&extern_table_size,&entry_table_size);
	}
	full_clean(label_table,extern_table, entry_table, instruction_table, data_table, hash_func_table, hash_saved_words, &label_table_true_size,&data_table_true_size,&instruction_table_true_size,&extern_table_true_size, &entry_table_true_size);
	printf("assembler work finished for %d files\n",argc - 1);
	return 0	;
}
