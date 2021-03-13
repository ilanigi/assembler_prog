#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "check_funcs.h"
#include "first_run.h"

/*put next code line into slots in a buffer*/
int get_next_line(FILE * file_ptr, char full_line[], int * line_code_num)
{
	int c;
	int i;
	int j;
		
	next_line:
	line_code_num++;
	i = j = 0;
	c = fgetc(file_ptr);

	while(isspace(c))
	{
		if (feof(file_ptr)) break;
		if (c == '\n')
		{
			/*that means this line is empty, we go stright to next line by running this code again*/
			goto next_line;
		}
		i++;
		c = fgetc(file_ptr);
	}

	 if (feof(file_ptr)) return 1;
	
	/*hendeling comments in file*/
	if (c == ';')
	{
		while(c != '\n')
		{
			if (feof(file_ptr)) return 1;
			c = fgetc(file_ptr);
		}
		goto next_line;
	}
	
	
	/*hendeling white spaces before first letters*/
	while(isspace(c))
	{
		if (feof(file_ptr)) break;
		if (c == '\n')
		{
			/*that means this line is empty, we go stright to next line by running this code again*/
			goto next_line;
		}
		i++;
		c = fgetc(file_ptr);
	}

	if (feof(file_ptr)) return 1;

	/*copy all line to the result string "full_line"*/
	while (i < MAX_LINE_SIZE && c != '\n' && !feof(file_ptr))
	{
		full_line[j] = c;
		j++;
		i++;
		c = fgetc(file_ptr);
	}
	full_line[j] = '\0';
	
	return 0;
}

/*put next file line into slots in an array, return 0 if line is valid, else -1*/
int line_to_slots(char full_line[],char *** slot_array,short unsigned int * label_flag, int * argument_num, int * slot_array_size, int line_code_num, char * file_name)
{
	short unsigned int i; 			/*full_line index*/
	short unsigned int j = 0;		/*slot_array index*/
	short unsigned int k;			/*buffer index*/
	short unsigned int has_error = 0;		
	short unsigned int before_argument = 0;	/*	all thos flags can...					*/
	short unsigned int in_argument = 1;		/*	become one number with only 2 bits,...	*/
	short unsigned int after_argument = 0;	/* (like a statment machine)...			*/
	short unsigned int ready_to_copy = 0;	/*	maybe we'll change it later.			*/
	char * buf;
	char ** temp_pointer;
	
	*label_flag = 0;
	
	if (!(buf = (char *) malloc(sizeof(char) * MAX_ARGUMENT_SIZE)))
	{
		printf("FATAL ERROR: no place in memory\n");
		return 1;
	}
	
	/*getting first argument to the slot*/
	for(i = 0, k = 0; full_line[i] != '\0';i++,k++)
	{
		if (full_line[i] == ':')
		{
			*label_flag = 1;
			break;
		}
		if (isspace(full_line[i])) break;

		buf[k] = full_line[i];	
	}
	
	buf[k] = '\0';
	(*slot_array)[j++] = buf;
	
	if (!(buf = (char *) malloc(sizeof(char) * MAX_ARGUMENT_SIZE)))
		{
			printf("FATAL ERROR: no place in memory\n");
			return 1;
		}
	
	
	/*if there is label, full_line[i] is ':' now, so we we move one index forward*/
	i = *label_flag ? (i+1):i; 
	/*not sure if next check is necessary*/
	if (!isspace(full_line[i]) && full_line[i] != '\0')
	{
		printf("ERROR in file %s\t in line %d:\t missing space between argument 1 and 2\n",file_name,line_code_num);
		has_error = 1;
	}
	while (isspace(full_line[i]))
		i++;
	/*getting second argument to the slot if label exsist*/
	if(*label_flag)
	{
		for(k = 0; !isspace(full_line[i]) && full_line[i] != '\0' ;i++,k++)
		{

			if (full_line[i] == ',') 
			{
				
				printf("ERROR in file %s\t in line %d:\tunnecessary ',' between arguments \n",file_name,line_code_num);
				has_error = 1;
				break;
			}
			buf[k] = full_line[i];
		}
	
		buf[k] = '\0';
		(*slot_array)[j++] = buf;
		
		while (isspace(full_line[i]))	i++;

		if (full_line[i] != '\0')
		{
			if (!(buf = (char *) malloc(sizeof(char)*MAX_ARGUMENT_SIZE)))
			{
				printf("FATAL ERROR: no place in memory\n");
				return 1;
			}
		}
	
	}	
	k = 0;
	/*if line has only 2 arguments - all flags are off*/
	in_argument =  (full_line[i] == '\0')?0:1;
	/*this loop enter all the rest of the argument to the slot_array*/
	while(full_line[i] != '\0' && full_line[i] != '\n')
	{
		if (before_argument)
		{	
			if (full_line[i] == ',')
			{
				has_error = 1;
				printf("ERROR in file %s\t in line %d:\tmissing argument\n",file_name,line_code_num);
			}
			else if (!isspace(full_line[i]))
			{
				in_argument = 1;
				before_argument = 0;
			}

		}

		if (in_argument)
		{
			if (full_line[i] == ',') 
			{
				in_argument = 0;
				ready_to_copy = 1;
			}
			else if (isspace(full_line[i]))
			{
				in_argument = 0;
				after_argument = 1;
			}
			else
			{
				buf[k++] = full_line[i];
			}
		}
		if (after_argument)
		{
			if (full_line[i] == ',')
			{
				after_argument = 0;
				ready_to_copy = 1;
			}
			else if (!isspace(full_line[i]))
			{
				printf("ERROR in file %s\t in line %d:\tmissing comma\n",file_name,line_code_num);
				after_argument = 0;
				has_error = 1;
				in_argument = 1;
				i--;
			}
		}
		if (ready_to_copy)
		{
			ready_to_copy = 0;
			before_argument = 1;
			buf[k] = '\0';
			/* cheack if we should increase the slot_array */ 
			if(j == *slot_array_size)
			{
				temp_pointer = (char **) realloc( *slot_array , (sizeof(**slot_array) + 4 * (sizeof(char*))));
				
				if (!temp_pointer)
				{
					printf("FATAL ERROR: no place in memory\n");
					return 1;
				}
				*slot_array = temp_pointer;
				*slot_array_size += 4;
			}
			(*slot_array)[j++] = buf;
			
			if (!(buf = (char *) malloc(sizeof(char) * MAX_ARGUMENT_SIZE)))
			{
				printf("FATAL ERROR: no place in memory\n");
				return 1;
			}
			
			k = 0;
		}
		i++;	
		}
		/*taking care of last argument*/
		if(in_argument)
		{
			before_argument = 0;
			buf[k] = '\0';
			if(j == *slot_array_size)
			{
				
				temp_pointer = (char **) realloc( slot_array , (sizeof(*slot_array) + sizeof(char*)));
				
				if (!temp_pointer)
				{
					printf("FATAL ERROR: no place in memory\n");
					return 1;
				}
				*slot_array = temp_pointer;
				*slot_array_size +=4;
			}

			(*slot_array)[j++] = buf;				
		}
		if(before_argument)
		{
			printf("ERROR in file %s\t in line %d:\tmissing argument\n",file_name,line_code_num);
			has_error = 1;
		}
		if(after_argument)
		{
			buf[k] = '\0';
			if(j == *slot_array_size)
			{
				
				temp_pointer = (char **) realloc( slot_array , (sizeof(*slot_array) + sizeof(char*)));
				
				if (!temp_pointer)
				{
					printf("FATAL ERROR: no place in memory\n");
					return 1;
				}
				*slot_array = temp_pointer;
				*slot_array_size +=4;
			}
			(*slot_array)[j++] = buf;				
		}
	*argument_num = j;
	return has_error;
}


/*
if opcode and funct are valid -  opcode is in funct_and_op[1] and funct in funct_and_op[0] 
else - put funct_and_op[0] = funct_and_op[1] = -1
*/

int get_str_size(char * str)
{
	int counter = 0;
	int i;
	for(i = 0; str[i] != '\0'; i++)
	{	
		counter++;
	}
	return counter;
}

/*checks if the label exciset and has opposite attribute ( extern if entry, and opossite) */
int valid_extrn_entry(char * label,label_table_line** label_tabel,int label_table_size,char * other_attribute)
{
	int i;
	for ( i = 0; i < label_table_size; i++)
	{
		if(!strcmp(label,label_tabel[i]->label) && !strcmp(other_attribute,label_tabel[i]->attribute))
			return 0;
	}
	return 1;
	
}

int first_run(label_table_line *** label_table, int * label_table_size,memory_table_line *** data_table,int * data_table_size, memory_table_line*** instruction_table,int * instruction_table_size, func_node** hash_func_table, node** hash_saved_words, FILE * fptr, int * ICF, int * DCF, int * has_entry, char * file_name)
{
	/* structs unique to this run */
	func_bit_filed func_value_table[16] = 
	{
		{0,0,2},
		{1,0,2},
		{2,10,2},
		{2,11,2},
		{4,0,2},
		{5,10,1},
		{5,11,1},
		{5,12,1},
		{5,13,1},
		{9,10,1},
		{9,11,1},
		{9,12,1},
		{12,0,1},
		{13,0,1},
		{14,0,0},
		{15,0,0}
	};

	node* hash_label_names[26] = {NULL};

	short unsigned int label_flag = 0;
	short unsigned int last_line = 0;
	
	int DC = 0;
	int IC = 100;
	int i;
	int j;
	int line_code_num = 0;		/*line number in original code file*/
	char ** slot_array;			/*slot array is an array,which in every cell a pointer to an argument in the code line*/
	char full_line[MAX_LINE_SIZE];
	unsigned short int has_error = 0;
	int arguments_num = 4;		/*number of arguments in a slot_array*/
	int slot_array_size = 4;
	int max_arg = 0;
	int data_flag = 0;
	word word_ptr = {0};
	short int first_arg;
	short int func_num;
	int operand_num1;
	int operand_num2;
	char ARE1;
	char ARE2;
	char label1[MAX_LABEL_SIZE+2];
	char label2[MAX_LABEL_SIZE+2];
	*has_entry = 0;

	/*allocating space for slot_array*/
	if (!(slot_array = (char **) malloc(sizeof(char *) * arguments_num))) 
	{
		printf("ERROR in file %s\t in line %d:\tno place in memory\n",file_name,line_code_num);
		return 1;
	}
	
	/*
	there is a problem here - if last line is empty - while loop shuold end - and it doen't...
	todo: FIX IT
	*/
	while (1)
	{
		*label1 = '\0';
		*label2 = '\0';
		data_flag = 0;
		last_line = get_next_line(fptr,full_line,&line_code_num);
		if (last_line) break;
		if(line_to_slots(full_line, &slot_array, &label_flag, &arguments_num, &slot_array_size, line_code_num,file_name)) has_error = 1;
		max_arg = max_arg < arguments_num ? arguments_num : max_arg;
		data_flag = is_data(label_flag, slot_array);
		/*
		to know where is the first argument we use the label_flag:
		if he's on - we add one to the place, else - 0
		*/
		first_arg = 1 + label_flag;	

		if( data_flag || is_string(label_flag, slot_array))
		{
			if(label_flag)
			{
				if(!is_valid_label(slot_array[0], hash_saved_words, hash_label_names))
				{
					printf("ERROR in file %s\t in line %d:\t invalid label name\n",file_name,line_code_num);
					has_error = 1;
				}
				add_line_to_label_table(label_table, DC, slot_array[0],slot_array[1], label_table_size);
				printf("label\t");
				printf("%s\t%d\t\t%s\n",(*label_table)[*label_table_size-1]->label,(*label_table)[*label_table_size-1]->value,(*label_table)[*label_table_size-1]->attribute);
			}

			if(data_flag)
			{	
				i = first_arg;
				/*add data arguments to data table*/
				while(i < arguments_num)
				{
					if (atoi(slot_array[i]) > 2047 || atoi(slot_array[i]) < -2048)
						{
							printf("ERROR in file %s\t in line %d:\toverflow - data is too big for 12 bit\n",file_name,line_code_num);
							has_error = 1;
						}
					
					put_immediate_to_word(&word_ptr, atoi(slot_array[i]));
					/*only first argument is list will get label name*/
					if (i == first_arg && label_flag) strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					if (!label_flag) strcpy(label1,"");
					add_line_to_memory_table(data_table,DC++, word_ptr,'A', label1, data_table_size,0);
					printf("data\t");
					if ((*data_table)[*data_table_size-1]->address <= 999)
					printf("0%d\t",(*data_table)[*data_table_size-1]->address);
					else
					printf("%d\t",(*data_table)[*data_table_size-1]->address);
				
					print_word((*data_table)[*data_table_size-1]->machine_code);
				
					printf("\t");
					
					printf("%c\t",(*data_table)[*data_table_size-1]->ARE);
				
					printf("%s\n",(*data_table)[*data_table_size-1]->label);
					i++;
				}
			}
			/*it is a .string statment*/
			else
			{	/* starting from index 1 and end at -2 to eliminate the "" */
				for(i=1; i < get_str_size(slot_array[first_arg])-1; i++)
				{			
					put_immediate_to_word(&word_ptr, slot_array[first_arg][i]);
					if (i == 1) 
						strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					add_line_to_memory_table(data_table, DC++, word_ptr,'A',label1, data_table_size,0);
					printf("data\t");
					if ((*data_table)[*data_table_size-1]->address <= 999)
					
						printf("0%d\t",(*data_table)[*data_table_size-1]->address);
					else
						printf("%d\t",(*data_table)[*data_table_size-1]->address);
				
					print_word((*data_table)[*data_table_size-1]->machine_code);
				
					printf("\t");
					
					printf("%c\t",(*data_table)[*data_table_size-1]->ARE);
				
					printf("%s\n",(*data_table)[*data_table_size-1]->label);
				
				}
				put_immediate_to_word(&word_ptr,0);
					if (i == 1) 
						strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					add_line_to_memory_table(data_table, DC++, word_ptr,'A',label1, data_table_size,0);
					printf("data\t");
					if ((*data_table)[*data_table_size-1]->address <= 999)
					
						printf("0%d\t",(*data_table)[*data_table_size-1]->address);
					else
						printf("%d\t",(*data_table)[*data_table_size-1]->address);
				
					print_word((*data_table)[*data_table_size-1]->machine_code);
				
					printf("\t");
					
					printf("%c\t",(*data_table)[*data_table_size-1]->ARE);
				
					printf("%s\n",(*data_table)[*data_table_size-1]->label);
			}
			continue;
		}
		if (is_entry(slot_array,label_flag,arguments_num)) 
		{
			if(!valid_extrn_entry(slot_array[first_arg],*label_table,*label_table_size,".extern"))
			{
				has_error = 1;
				printf("ERROR in file %s\t in line %d:\tentry label is already an external label\n",file_name,line_code_num);
			}
			add_line_to_label_table(label_table,0,slot_array[first_arg], ".entry", label_table_size);
			printf("label\t");
			printf("%s\t%d\t\t%s\n",(*label_table)[*label_table_size-1]->label,(*label_table)[*label_table_size-1]->value,(*label_table)[*label_table_size-1]->attribute);
			*has_entry = 1;
			continue;
		}
		if (is_extern(slot_array,label_flag,arguments_num))
		{
			if (label_flag)
			{
				printf("WARNING in file %s\t in line %d:\t.extern value has label\n",file_name,line_code_num);
				continue;
			}
			if (!is_valid_label(slot_array[first_arg], hash_saved_words, hash_label_names))	
			{
				printf("ERROR in file %s\t in line %d:\t illigal label name\n",file_name,line_code_num);
			}
			if(!valid_extrn_entry(slot_array[first_arg],*label_table,*label_table_size,".entry"))
			{
				has_error = 1;
				printf("ERROR in file %s\t in line %d:\textern label is already an entry label\n",file_name,line_code_num);
			}

			add_line_to_label_table(label_table, 0, slot_array[first_arg],".extern",label_table_size);
			printf("label\t");
			printf("%s\t%d\t\t%s\n",(*label_table)[*label_table_size-1]->label,(*label_table)[*label_table_size-1]->value,(*label_table)[*label_table_size-1]->attribute);

			continue;		
		}
		/* it is a code line */
		if(label_flag)
		{	
			/*add label to label table*/
			if(!is_valid_label(slot_array[0], hash_saved_words, hash_label_names))
			{
				printf("ERROR in file %s\t in line %d:\tinvalid label name\n",file_name,line_code_num);
				has_error = 1;
			}
			add_line_to_label_table(label_table, IC, slot_array[0], "code", label_table_size);
			printf("label\t");
			printf("%s\t%d\t\t%s\n",(*label_table)[*label_table_size-1]->label,(*label_table)[*label_table_size-1]->value,(*label_table)[*label_table_size-1]->attribute);
		}

		/*check for a valid function name*/
		if ( -1 == (func_num = is_valid_func(hash_func_table, slot_array[label_flag])))
		{
			printf("ERROR in file %s\t in line %d:\tinvalid func name\n",file_name,line_code_num);
			has_error = 1;
			continue;
		}

		/*insert funct and opcode to the code */
		word_ptr.funct = func_value_table[func_num].funct;
		word_ptr.opcode = func_value_table[func_num].opcode;
		
		/*
		This next switch detect operands values by amout,
		and  insert them and the code line to the instruction
		*/
		
		switch (func_value_table[func_num].operands)
		{
		/*no arguments in the func*/
		case 0:
			{
			word_ptr.source = 0;
			word_ptr.destenation = 0;
			add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A', "", instruction_table_size,line_code_num);
			printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
				printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
			else
				printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
		
			print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
		
			printf("\t");
			
			printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
		
			printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);
			break;
			}
		case 1:
			{
				/*operand is a imidiant value*/
				if (slot_array[first_arg][0] == '#' && is_valid_imd(&(slot_array[first_arg][1])))
				{
					word_ptr.destenation = 0;

					operand_num1 = atoi(&(slot_array[first_arg][1]));

					ARE1 = 'A';
				}
			else if (is_legal_reg(slot_array[first_arg]))
				{
					word_ptr.destenation = 3;
					operand_num1 = (1 << atoi(&(slot_array[first_arg][1])));
					ARE1 = 'A';
				}
				/* string without first letter (which is '%')*/
			else if (slot_array[first_arg][0] == '%' )
				{
					/*adding distance from code line to label name*/
					strcpy(label1,"%1");
					strcat(label1,&(slot_array[first_arg][1]));
					operand_num1 = 0;
					word_ptr.destenation = 2;
					ARE1 = '?';
				}
				/*operand is a label*/
			else 
				{
					strcpy(label1,slot_array[first_arg]);
					word_ptr.destenation = 1;
					operand_num1 = 0;
					ARE1 = '?';
				}

				add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A',"", instruction_table_size,line_code_num);
				printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
					printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
				else
					printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
			
				print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
			
				printf("\t");
				
				printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
			
				printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);
				put_immediate_to_word(&word_ptr,operand_num1); 
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE1, label1, instruction_table_size,line_code_num);
				printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
				printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
				else
					printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
			
				print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
			
				printf("\t");
				
				printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
			
				printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);
				break;
			}
				
		case 2:
			{
				/*first operand analasys*/
				/*operand is a imidiant value*/
				if (slot_array[first_arg][0] == '#' && is_valid_imd(&(slot_array[first_arg][1])))
				{
					word_ptr.source = 0;
					operand_num1 = atoi(slot_array[first_arg]);
					ARE1 = 'A';
				}
			else if (is_legal_reg(slot_array[first_arg]))
				{
					word_ptr.source = 3;
					operand_num1 = (1 << atoi(&(slot_array[first_arg][1])));
					ARE1 = 'A';
				}
				/* string without first letter (which is '%') */
			else if (slot_array[first_arg][0] == '%' )
				{
					strcpy(label1,"%1");
					strcat(label1,&(slot_array[first_arg][1]));
					operand_num1 = 0;
					word_ptr.source = 2;
					ARE1 = '?';
				}
				/*operand is a label*/
				else
				{
					strcpy(label1,slot_array[first_arg]);
					word_ptr.source = 1;
					operand_num1 = 0;
					ARE1 = '?';
				}
				/*second operand analasys*/
				/*operand is a imidiant value*/
				if (slot_array[first_arg + 1][0] == '#' && is_valid_imd(&(slot_array[first_arg + 1][1])))
				{
					operand_num2 = atoi(&(slot_array[first_arg+1][1]));
					word_ptr.destenation = 0;
					ARE2 = 'A';
				}
			else if (is_legal_reg(slot_array[first_arg + 1]))
				{
					operand_num2 = ( 1 << atoi(&(slot_array[first_arg + 1][1])));
					word_ptr.destenation = 3;
					ARE2 = 'A';
				}
					/* string without first letter (which is '%') */
			else if ( slot_array[first_arg + 1][0] == '%' )
				{
					strcpy(label2,"%2");
					strcat(label2,&(slot_array[first_arg][1]));
					operand_num2 = 0;
					word_ptr.destenation = 2;
					ARE2 = '?';
				}
				/*operand is a label*/
				else
				{
					strcpy(label2,slot_array[first_arg + 1]);
					word_ptr.destenation = 1;
					operand_num2 = 0;	
					ARE2 = '?';
				}
				
				add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A', "", instruction_table_size,line_code_num);
				printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
					printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
				else
					printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
			
				print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
			
				printf("\t");
				
				printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
			
				printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);
				
				put_immediate_to_word(&word_ptr,operand_num1);
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE1, label1, instruction_table_size,line_code_num);
				printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
					printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
				else
					printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
			
				print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
			
				printf("\t");
				
				printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
	
				printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);
				
				put_immediate_to_word(&word_ptr,operand_num2);
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE2, label2, instruction_table_size,line_code_num);
				printf("instrc\t");
				if ((*instruction_table)[*instruction_table_size-1]->address <= 999)
					printf("0%d\t",(*instruction_table)[*instruction_table_size-1]->address);
				else
					printf("%d\t",(*instruction_table)[*instruction_table_size-1]->address);
	
				print_word((*instruction_table)[*instruction_table_size-1]->machine_code);
			
				printf("\t");
				
				printf("%c\t",(*instruction_table)[*instruction_table_size-1]->ARE);
			
				printf("%s\n",(*instruction_table)[*instruction_table_size-1]->label);

			
			}		
		}/*end of switch*/
	}
	*ICF = IC;
	*DCF = DC;
	/*todo - move slot_array to init, and free it after all files done*/
	printf("end of file has been reched\n");
	for(i = 0; i < max_arg; i++)
		{
			free(slot_array[i]);
		}
	free(slot_array);

	printf("after first run:\n");
	print_memory_table((*instruction_table), *instruction_table_size);
	printf("\n");
	print_memory_table((*data_table), *data_table_size);
	printf("\n");
	print_label_table((*label_table),*label_table_size);
	printf("\n");
	return has_error;
}
