#define MAX_LINE_SIZE 80
#define MAX_ARGUMENT_SIZE 80
/*-----------------------func-table--------------------------*/

/*this will help convert a known func number to it's bits values in a word*/

typedef struct {
	unsigned int opcode : 4;
   	unsigned int funct : 4;
   	unsigned int operands : 2;
} func_bit_filed ;

/*--------------------functions-declerations----------------------*/

int get_next_line(FILE * file_ptr, char full_line[], int * line_code_num);

int line_to_slots(char full_line[],char *** slot_array,short unsigned int * label_flag, int * argument_num, int * slot_array_size, int line_code_num, char * file_name);

int get_str_size(char * str);

int first_run(label_table_line *** label_table, int * label_table_size,memory_table_line *** data_table,int * data_table_size, memory_table_line*** instruction_table,int * instruction_table_size, func_node** hash_func_table, node** hash_saved_words, FILE * fptr, int * ICF, int * DCF, int * has_entry, char * file_name);

