#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "first_run.h"
#include "init.h"
#include "sec_run.h"
#include "print_to_file.h"
#include "main.h"

int main( int argc, char *argv[])
{
	/*init hash tabels*/
	func_node* hash_func_table[FUNCS_AOUNT] = {0};
	node* hash_saved_words[HASH_TABLE_SIZE] = {0};
	/*init tables*/
	label_table_line ** label_table = {0};
	memory_table_line** instruction_table = {0};
	memory_table_line** data_table = {0};
	label_table_line** extern_tabel = {0};
	int label_table_size = 0;
	int data_table_size = 0;
	int instruction_table_size = 0;
	int extern_tabel_size = 0;
	/*int i = 1;*/
	int ICF;
	int DCF;
	/* this  int checks if a given file has entry values */
	int has_entry;
	FILE * fptr;
	int has_error;
	char file_name[MAX_FILE_NAME_SIZE];
	
	/*init hash tables*/
	printf("starting init\n");
	init_hash(hash_func_table, hash_saved_words);
	/*
	for (i = 1; i < argc; i++)	
	{

		/*open file to fptr*/
		/*
		strcpy(file_name,argv[i]);
		strcat(file_name,".as");
		if (!(fptr = fopen(file_name,"r")))
		{
			printf("file is not exsist");
			continue;
		}*/
		printf("starting file\n");
		if (!(fptr = fopen("C:\\Users\\moria ben ishay\\Desktop\\assembler2\\file_exmpl.txt","r")))
		{
			printf("file %s is not exsist\n"/*,argcv[i]*/);
		}
		printf("starting first_run\n");
		
		has_error = first_run(&label_table, &label_table_size, &data_table, &data_table_size, &instruction_table, &instruction_table_size, hash_func_table, hash_saved_words, fptr, &ICF,&DCF, &has_entry,"argv[i]");
		has_error |= sec_run(&label_table, &label_table_size, &data_table, &data_table_size, &instruction_table, &instruction_table_size, hash_func_table,&extern_tabel,&extern_tabel_size, ICF,"argv[i]");
		if(!has_error)
		{
			printf("printing\n");
			if (print_to_files("C:\\Users\\moria ben ishay\\Desktop\\assembler2\\file_exmpl",data_table, data_table_size, instruction_table, instruction_table_size, label_table, label_table_size, extern_tabel,extern_tabel_size, ICF, DCF, has_entry))
			printf("error\n");	
		}
		else
		{
			
		}
		fclose(fptr);
		
	/*printf("file %s is done",argv[i]);
	}*/
	return 0;
}