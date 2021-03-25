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
	(*line_code_num)++;
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
int line_to_slots(char full_line[],char *** slot_array,short int * label_flag, int * argument_num, int * slot_array_size, int line_code_num, char * file_name)
{
	short int i; 					/*full_line index*/
	short int j = 0;				/*slot_array index*/
	short int k;					/*buffer index*/
	short int error_counter = 0;		
	short int state = in_argument; 	/* as in enum*/
	short int ready_to_copy = 0;
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

	if(*label_flag && strlen(buf) > MAX_LABEL_SIZE)
	{
		error_counter++;
		printf("ERROR in file %s\t in line %d:\t label size is bigger then possible size\n",file_name,line_code_num);
	}

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
		error_counter++;
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
				
				printf("ERROR in file %s\t in line %d:\tunnecessary comma between arguments \n",file_name,line_code_num);
				error_counter++;
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
	state = (full_line[i] == '\0') ? none : in_argument;
	/*this loop enter all the rest of the argument to the slot_array*/
	while(full_line[i] != '\0' && full_line[i] != '\n')
	{
		if (state == before_argument)
		{	
			if (full_line[i] == ',')
			{
				error_counter++;
				printf("ERROR in file %s\t in line %d:\tmissing operand before comma\n",file_name,line_code_num);
			}
			else if (full_line[i] == '"')
			{
				state = string;
			}
			else if (!isspace(full_line[i]))
			{
				state = in_argument;
			}

		}

		if (state == in_argument)
		{
			if (full_line[i] == ',') 
			{
				state = none;
				ready_to_copy = 1;
			}
			else if (full_line[i] == '"')
			{
				state = string;
			}
			else if (isspace(full_line[i]))
			{
				state = after_argument;
			}
			else
			{
				buf[k++] = full_line[i];
			}
		}
		if (state == after_argument)
		{
			if (full_line[i] == ',')
			{
				state = none;
				ready_to_copy = 1;
			}
			else if (!isspace(full_line[i]))
			{
				printf("ERROR in file %s\t in line %d:\tmissing comma\n",file_name,line_code_num);
				state = in_argument;
				error_counter++;
				i--;
			}
		}
		if (state == string)
		{
			buf[k++] = full_line[i++];
			while(full_line[i] != '"' && full_line[i]!= '\0')	
			{
				buf[k++] = full_line[i++];
			}
			ready_to_copy = 1;

			if(full_line[i] != '"')
			{
				printf("ERROR in file %s\t in line %d:\tmissing quotation mark in end of instruction\n",file_name,line_code_num);
				error_counter++;
			}
			else
				buf[k++] = full_line[i];

			i++;
			while(full_line[i] != '\0')
			{
				if (!isspace(full_line[i]))
				{
					printf("ERROR in file %s\t in line %d:\tinvalud argument after string instruction\n",file_name,line_code_num);
					error_counter++;
					while(full_line[i] != '\0' && !isspace(full_line[i]))
					i++;
				}
				i++;
			}

		}
		if (ready_to_copy)
		{
			ready_to_copy = 0;
			buf[k] = '\0';
			/* cheack if we should increase the slot_array */ 
			if(j == *slot_array_size)
			{
				temp_pointer = (char **) realloc( *slot_array , ((*slot_array_size) * sizeof(char *)) + sizeof(char *) );
				
				if (!temp_pointer)
				{
					printf("FATAL ERROR: no place in memory\n");
					return 1;
				}
				*slot_array = temp_pointer;
				*slot_array_size += 1;
			}
			(*slot_array)[j++] = buf;
			
			if (!(buf = (char *) malloc(sizeof(char) * MAX_ARGUMENT_SIZE)))
			{
				printf("FATAL ERROR: no place in memory\n");
				return 1;
			}
			
			
			k = 0;
			if (state == string)
				goto end_string;
			state = before_argument;
		}
		i++;	
		}
		/*taking care of last argument*/
		switch (state)
		{
				case in_argument:
			{
				buf[k] = '\0';
				if(j == *slot_array_size)
				{
					temp_pointer = (char **) realloc( *slot_array , ((*slot_array_size) * sizeof(char *)) + sizeof(char *) );
					if (!temp_pointer)
					{
						printf("FATAL ERROR: no place in memory\n");
						return 1;
					}
				
					*slot_array = temp_pointer;
					*slot_array_size += 1;
				}

				(*slot_array)[j++] = buf;				
				break;
			}
			case before_argument:
			{
				printf("ERROR in file %s\t in line %d:\tundefined comma found\n",file_name,line_code_num);
				error_counter++;
				break;
			}
			case after_argument:
			{
				buf[k] = '\0';
				if(j == *slot_array_size)
				{					
					temp_pointer = (char **) realloc( *slot_array , ((*slot_array_size) * sizeof(char *)) + sizeof(char *) );
					
					if (!temp_pointer)
					{
						printf("FATAL ERROR: no place in memory\n");
						return 1;
					}
					*slot_array = temp_pointer;
					*slot_array_size +=1;
				}
				(*slot_array)[j++] = buf;				
				break;
			}
		}
	end_string:
	*argument_num = j;
	return error_counter;
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
int valid_extrn_entry(char * label,label_table_line** label_table,int label_table_size, int other_attribute)
{
	int i;
	for ( i = 0; i < label_table_size; i++)
	{
		if(!strcmp(label,label_table[i]->label) && other_attribute == label_table[i]->attribute )
			return 0;
	}
	return 1;
	
}

int first_run(label_table_line *** label_table, int * label_table_size,memory_table_line *** data_table,int * data_table_size, memory_table_line*** instruction_table,int * instruction_table_size, func_node** hash_func_table, node** hash_saved_words, int * label_table_true_size, int * data_table_true_size, int * instruction_table_true_size ,label_table_line *** entry_table,int * entry_table_size,int * entry_table_true_size,label_table_line *** extern_table,int * extern_table_size,int * extern_table_true_size,FILE * fptr, int * ICF, int * DCF, char * file_name)
{
	/* structs unique to this run */
	func_bit_filed func_value_table[FUNCS_AMOUNT] = 
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

	node* hash_label_names[HASH_TABLE_SIZE] = {NULL};

	short int label_flag = 0;
	short int last_line = 0;
	int DC = 0;
	int IC = 100;
	int i;
	int line_code_num = 0;		/*line number in original code file*/
	char ** slot_array;			/*slot array is an array,which in every cell a pointer to an argument in the code line*/
	char full_line[MAX_LINE_SIZE];
	unsigned short int error_counter = 0;
	int arguments_num = 4;		/*number of arguments in the code line*/
	int slot_array_size = 4;
	int max_arg = 4;
	int data_flag = 0;
	word word_ptr = {0};
	short int first_arg;
	short int func_num;
	short int missing_arg;
	short int valid_label_num;
	short int temp_attribute;
	int str_size;
	int operand_num1;
	int operand_num2;
	char ARE1;
	char ARE2;
	char label1[MAX_LABEL_SIZE+2];
	char label2[MAX_LABEL_SIZE+2];


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
		missing_arg = 0;
		*label1 = '\0';
		*label2 = '\0';
		data_flag = 0;
		last_line = get_next_line(fptr,full_line,&line_code_num);
		if (last_line) break;
		error_counter += line_to_slots(full_line, &slot_array, &label_flag, &arguments_num, &slot_array_size, line_code_num,file_name);
		max_arg = max_arg < arguments_num ? arguments_num : max_arg;
		data_flag = is_data(label_flag, slot_array);
		temp_attribute = 0;
		/*
		to know where is the first argument we use the label_flag:
		if he's on - we add one to the place, else - 0
		*/
		first_arg = 1 + label_flag;	

		if( data_flag || is_string(label_flag, slot_array))
		{
			if(label_flag)
			{
				valid_label_num = is_valid_label(slot_array[0], hash_saved_words, hash_label_names);
				insert_to_hash_table(hash_function(slot_array[0],HASH_TABLE_SIZE), slot_array[0], hash_label_names);
				if(valid_label_num == alredy_exsiet)
				{
					printf("ERROR in file %s\t in line %d:\tlabel \"%s\" alredy exsiet in file\n",file_name,line_code_num,slot_array[0]);
					error_counter++;
				}
				else if (valid_label_num == in_saved_words )
				{
					printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
					error_counter++;
				}
				else if (valid_label_num == imd_letter )
				{
					printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
					error_counter++;
				}
				
				temp_attribute = data_flag ? Data : String;
				add_line_to_label_table(label_table, DC, slot_array[0], temp_attribute, label_table_size, label_table_true_size);
				
			}

			if(data_flag)
			{	
				i = first_arg;
				/*i'm not sure if .data line without argumets is valid or not*/
				if(arguments_num - label_flag < 2 )
				{
					printf("WARNING in file %s\t in line %d:\t.data has no arguments\n",file_name,line_code_num);
					/*error_counter++;*/
				}
				/*add data arguments to data table*/
				while(i < arguments_num)
				{
					if(!is_valid_imd(slot_array[i]))
					{
						printf("ERROR in file %s\t in line %d:\tinvalid immediate: \"%s\"\n",file_name,line_code_num,slot_array[i]);
						error_counter++;
					}
					if (atoi(slot_array[i]) > 2047 || atoi(slot_array[i]) < -2048)
						{
							printf("ERROR in file %s\t in line %d:\toverflow: \"%s\" is too big for 12 bit\n",file_name,line_code_num,slot_array[i]);
							error_counter++;
						}
					
					put_immediate_to_word(&word_ptr, atoi(slot_array[i]));
					/*only first argument is list will get label name*/
					if (i == first_arg && label_flag) strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					if (!label_flag) strcpy(label1,"");
					add_line_to_memory_table(data_table,DC++, word_ptr,'A', label1, data_table_size,data_table_true_size,0);
					
					i++;
				}
			}
			/*it is a .string statment*/
			else
			{	/* starting from index 1 and end at -2 to eliminate the "" */
				str_size = get_str_size(slot_array[first_arg]);
				if(slot_array[first_arg][0] != '"' || slot_array[first_arg][str_size-1] != '"') 
				{
					printf("ERROR in file %s\t in line %d:\t.string value is missing quotation mark\n",file_name,line_code_num);
					error_counter++;
				}
				for(i=1; i < str_size - 1; i++)
				{			
					put_immediate_to_word(&word_ptr, slot_array[first_arg][i]);
					if (i == 1) 
						strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					add_line_to_memory_table(data_table, DC++, word_ptr,'A',label1, data_table_size,data_table_true_size,0);
					
				}
				put_immediate_to_word(&word_ptr,0);
					if (i == 1) 
						strcpy(label1,slot_array[0]);
					else strcpy(label1,"");
					add_line_to_memory_table(data_table, DC++, word_ptr,'A',label1, data_table_size,data_table_true_size,0);
					
			}
			continue;
		}
		if (is_entry(slot_array,label_flag,arguments_num)) 
		{
			if (label_flag)
			{
				printf("WARNING in file %s\t in line %d:\t.extern value has label\n",file_name,line_code_num);
			}

			/*check valid argument number*/
			if(arguments_num - label_flag < 2 )
			{
				printf("ERROR in file %s\t in line %d:\tmissing arguments\n",file_name,line_code_num);
				error_counter++;	
			}
			if(arguments_num - label_flag > 2 )
			{
				printf("ERROR in file %s\t in line %d:\ttoo many arguments\n",file_name,line_code_num);
				error_counter++;
			}
			if(!valid_extrn_entry(slot_array[first_arg],*extern_table,*extern_table_size, Extern))
			{
				error_counter++;
				printf("ERROR in file %s\t in line %d:\tentry label is already an external label\n",file_name,line_code_num);
			}
			add_line_to_label_table(entry_table,line_code_num,slot_array[first_arg], Entry, entry_table_size,entry_table_true_size);
			continue;
		}
		
		if (is_extern(slot_array,label_flag,arguments_num))
		{
		
			if (label_flag)
			{
				printf("WARNING in file %s\t in line %d:\t.extern value has label\n",file_name,line_code_num);
			}
			/*check valid argument number*/
			if(arguments_num - label_flag < 2 )
			{
				printf("ERROR in file %s\t in line %d:\tmissing arguments\n",file_name,line_code_num);
				error_counter++;	
			}
			if(arguments_num - label_flag > 2 )
			{
				printf("ERROR in file %s\t in line %d:\ttoo many arguments\n",file_name,line_code_num);
				error_counter++;
			}

			valid_label_num = is_valid_label(slot_array[first_arg], hash_saved_words, hash_label_names);
			
			if(valid_label_num == alredy_exsiet)
			{
				printf("ERROR in file %s\t in line %d:\tlabel \"%s\" alredy exsiet in file\n",file_name,line_code_num,slot_array[first_arg]);
				error_counter++;
			}
			else if (valid_label_num == in_saved_words )
			{
				printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
				error_counter++;
			}
			

			if(!valid_extrn_entry(slot_array[first_arg],*entry_table,*entry_table_size, Entry))
			{
				error_counter++;
				printf("ERROR in file %s\t in line %d:\textern label is already an entry label\n",file_name,line_code_num);
			}
			
			add_line_to_label_table(extern_table, IC, slot_array[first_arg], 0, extern_table_size,extern_table_true_size);
			
			continue;		
		}
		/* it is a code line */
		if(label_flag)
		{	
			/*add label to label table*/
			
			valid_label_num = is_valid_label(slot_array[0], hash_saved_words, hash_label_names);
			insert_to_hash_table(hash_function(slot_array[0],HASH_TABLE_SIZE), slot_array[0],hash_label_names);
			
			if(valid_label_num == alredy_exsiet)
			{
				printf("ERROR in file %s\t in line %d:\tlabel \"%s\" alredy exsiet in file\n",file_name,line_code_num,slot_array[0]);
				error_counter++;
			}
			
			else if (valid_label_num == in_saved_words )
			{
				printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
				error_counter++;
			}
			add_line_to_label_table(label_table, IC, slot_array[0], Code, label_table_size,label_table_true_size);
			
		}

		/*check for a valid function name*/
		if ( -1 == (func_num = is_valid_func(hash_func_table, slot_array[label_flag])))
		{
			printf("ERROR in file %s\t in line %d:\t\"%s\" is an invalid func name\n",file_name,line_code_num,slot_array[label_flag]);
			error_counter++;
			continue;
		}
		/*check valid argument number*/
		/*(arguments_num - first_arg) is number of operands in the code line*/
		if(arguments_num - first_arg < func_value_table[func_num].operands)
		{
			printf("ERROR in file %s\t in line %d:\tmissing argument\n",file_name,line_code_num);
			error_counter++;
			missing_arg = 1;

		}
		if(arguments_num - first_arg > func_value_table[func_num].operands)
		{
			printf("ERROR in file %s\t in line %d:\ttoo many arguments\n",file_name,line_code_num);
			error_counter++;
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
			add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A', "", instruction_table_size,instruction_table_true_size,line_code_num);
			break;
			}
		case 1:
			{
				/*operand is a immediate value*/
				if (slot_array[first_arg][0] == '#')
				{
					if(func_num != prn && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid destenation operan (immediate) to \"%s\" function\n",file_name,line_code_num,slot_array[label_flag]);
						error_counter++;
					}
					if(!is_valid_imd(&(slot_array[first_arg][1])) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid immediate \"%s\"\n",file_name,line_code_num,slot_array[first_arg]);
						error_counter++;
					}
					
					word_ptr.destenation = 0;
					operand_num1 = atoi(&(slot_array[first_arg][1]));
					ARE1 = 'A';
				}
			else if (is_legal_reg(slot_array[first_arg]))
				{
					if((func_num <= jsr && func_num >= jmp ) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid destenation operan (register) to \"%s\" function\n",file_name,line_code_num,slot_array[label_flag]);
						error_counter++;
					}
					word_ptr.destenation = 3;
					operand_num1 = (1 << atoi(&(slot_array[first_arg][1])));
					ARE1 = 'A';
				}
				/* string without first letter (which is '%')*/
			else if (slot_array[first_arg][0] == '%' )
				{
					if((func_num <= dec || func_num >= red ) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid destenation operan (relative) to \"%s\" function\n",file_name,line_code_num,slot_array[label_flag]);
						error_counter++;
					}
					valid_label_num = is_valid_label(&slot_array[first_arg][1],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
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
					valid_label_num = is_valid_label(slot_array[first_arg],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}	
					strcpy(label1,slot_array[first_arg]);
					word_ptr.destenation = 1;
					operand_num1 = 0;
					ARE1 = '?';
				}

				add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A',"", instruction_table_size,instruction_table_true_size,line_code_num);
				put_immediate_to_word(&word_ptr,operand_num1); 
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE1, label1, instruction_table_size,instruction_table_true_size,line_code_num);
				
				break;
			}
				
		case 2:
			{
				/*source operand analasys*/
				/*operand is a immediate value*/
				if (slot_array[first_arg][0] == '#')
				{
					if (func_num == lea && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid source operan (immediate) to \"lea\" function\n",file_name,line_code_num);
						error_counter++;
					}
					if (!is_valid_imd(&(slot_array[first_arg][1])) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid immediate\n",file_name,line_code_num);
						error_counter++;
					}
					if ((atoi(&(slot_array[first_arg][1])) > MAX_IMDIANT || atoi(&(slot_array[first_arg][1])) < MIN_IMDIANT) && !missing_arg)
						{
							printf("ERROR in file %s\t in line %d:\toverflow: \"%s\" is too big for 12 bit\n",file_name,line_code_num,&slot_array[first_arg][1]);
							error_counter++;
						}
					

					word_ptr.source = 0;
					operand_num1 = atoi(slot_array[first_arg]);
					ARE1 = 'A';
				}
				else if (is_legal_reg(slot_array[first_arg]))
				{
					if (func_num == lea && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid source operan (register) to \"lea\" function\n",file_name,line_code_num);
						error_counter++;
					}
					word_ptr.source = 3;
					operand_num1 = (1 << atoi(&(slot_array[first_arg][1])));
					ARE1 = 'A';
				}
				
				/* string without first letter (which is '%') */
			else if (slot_array[first_arg][0] == '%' )
				{
					printf("ERROR in file %s\t in line %d:\tinvalid source operan (relative) to \"%s\" function\n",file_name,line_code_num, slot_array[label_flag]);
					error_counter++;
					valid_label_num = is_valid_label(&slot_array[first_arg][1],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					strcpy(label1,"%1");
					strcat(label1,&(slot_array[first_arg][1]));
					operand_num1 = 0;
					word_ptr.source = 2;
					ARE1 = '?';
				}
				/*operand is a label*/
				else
				{
					valid_label_num = is_valid_label(slot_array[first_arg],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					strcpy(label1,slot_array[first_arg]);
					word_ptr.source = 1;
					operand_num1 = 0;
					ARE1 = '?';
				}

				/*destenation operand analasys*/
				/*operand is a immediate value*/
				if (slot_array[first_arg + 1][0] == '#')
				{
					/*chekcing if both operand are immediates*/
					if (func_num != cmp && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid destenation operand (immediate) to \"%s\" function\n",file_name, line_code_num, slot_array[label_flag]);
						error_counter++;
					}
					/*chekcing if both operand are immediates*/
					else if ((slot_array[first_arg][0] == '#' && func_num != 1) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tfunction is not \"cmp\" and both operands are immediates\n",file_name,line_code_num);
						error_counter++;
					}
					
					/*cheking valid number*/
					if (!is_valid_imd(&(slot_array[first_arg + 1][1])) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\tinvalid immediate\n",file_name,line_code_num);
						error_counter++;
					}
					/*cheking overflow*/
					if ((atoi(&(slot_array[first_arg + 1][1])) > 2047 || atoi(&(slot_array[first_arg + 1][1])) < -2048) && !missing_arg)
					{
						printf("ERROR in file %s\t in line %d:\toverflow: \"%s\" is too big for 12 bit\n",file_name,line_code_num,&slot_array[first_arg + 1][1]);
						error_counter++;
					}
				
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
					printf("ERROR in file %s\t in line %d:\tinvalid source operan (relative) to \"%s\" function\n",file_name,line_code_num,slot_array[label_flag]);
					error_counter++;
					valid_label_num = is_valid_label(&slot_array[first_arg+1][1],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					strcpy(label2,"%2");
					strcat(label2,&(slot_array[first_arg][1]));
					operand_num2 = 0;
					word_ptr.destenation = 2;
					ARE2 = '?';
				}
				/*operand is a label*/
				else
				{
					valid_label_num = is_valid_label(slot_array[first_arg+1],hash_saved_words,hash_label_names);
					if (valid_label_num == in_saved_words )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" is a saved word\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					else if (valid_label_num == imd_letter )
					{
						printf("ERROR in file %s\t in line %d:\tlabel \"%s\" has a digit in his first letter\n",file_name,line_code_num,slot_array[0]);
						error_counter++;
					}
					strcpy(label2,slot_array[first_arg + 1]);
					word_ptr.destenation = 1;
					operand_num2 = 0;	
					ARE2 = '?';
				}
				
				add_line_to_memory_table(instruction_table, IC++, word_ptr, 'A', "", instruction_table_size,instruction_table_true_size,line_code_num);
				
				
				put_immediate_to_word(&word_ptr,operand_num1);
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE1, label1, instruction_table_size,instruction_table_true_size,line_code_num);
				
				
				put_immediate_to_word(&word_ptr,operand_num2);
				add_line_to_memory_table(instruction_table, IC++, word_ptr, ARE2, label2, instruction_table_size, instruction_table_true_size,line_code_num);
				
			
			}		
		}/*end of switch*/
	}
	*ICF = IC;
	*DCF = DC;
	
	for(i = 0; i < arguments_num ; i++)
		{
			free(slot_array[i]);
		}
		free(slot_array);
	erase_hash_table(hash_label_names,HASH_TABLE_SIZE);
	return error_counter;
}
