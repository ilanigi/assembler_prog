#define MAX_LABEL_SIZE 80
#define MAX_ATTRIBUTES_SIZE 20
#define MAX_CODE_LINE_SIZE 80
#define MAX_FILE_NAME_SIZE 80
/*-----------------------a-single-word--------------------------*/

typedef struct {
   unsigned int destenation : 2;
   unsigned int source : 2;
   unsigned int funct : 4;
   unsigned int opcode : 4;
} word;

void print_word(word word_toprint);

void put_immediate_to_word(word * current,int num);

/*-----------------------label-table--------------------------*/

/*line structure in the table.*/
typedef struct {
    char label[MAX_LABEL_SIZE];
    int value;
    char attribute[MAX_ATTRIBUTES_SIZE];
}label_table_line;

 void add_line_to_label_table(label_table_line *** current, int new_value, char new_symbol[], char new_attribute[],int * label_table_size);

void print_label_table(label_table_line ** table,int label_table_size);

void erase_label_table(label_table_line ** table,int * label_table_size);

/*--------------------------memory-table------------------------------*/

/*line structure in the table.*/

typedef struct {
    short int address;
	word machine_code;
    char ARE;
	char label[MAX_LABEL_SIZE];
    int line_code_num;
}memory_table_line;

void add_line_to_memory_table(memory_table_line *** current_table, int new_address, word new_machine_code ,char new_ARE,char new_label[], int * memory_table_size, int new_line_code_num);

void print_memory_table(memory_table_line ** table,int memory_table_size);

/*--------------------------hash-table----------------------------*/

typedef struct node
{
    char name[MAX_LABEL_SIZE];
    struct node* next;
}
node;

typedef struct func_node
{
    char name[MAX_ATTRIBUTES_SIZE];
	int num;
    struct func_node* next;
}
func_node;

int hash_function(char * input,int table_size);

void insert_to_hash_table(int key, char* buf,node** hash_table);

void insert_to_func_hash_table(int key, char* buf, short int num, func_node** hash_table);

int is_in_hash_table(node ** hash_table,int table_size, char* buf);

int is_valid_func(func_node ** hash_table, char* buf);
