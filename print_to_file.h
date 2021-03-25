int print_to_files(char * current_file_name, memory_table_line ** data_table, int data_table_size, memory_table_line ** instruction_table, int instruction_table_size,label_table_line ** label_table, int label_table_size, label_table_line ** extern_table, int extern_table_size,label_table_line ** entry_table, int entry_table_size ,int ICF, int DCF);

void print_word_to_file(word word_toprint, FILE * file_ptr);

void print_table_to_file(memory_table_line** table,int table_size,FILE * file_ptr, int ICF, int DCF);

void print_to_extern(label_table_line ** extern_table, int extern_table_size,FILE * file_ptr);

void print_to_entery(label_table_line ** label_table, int label_table_size, FILE * file_ptr);

