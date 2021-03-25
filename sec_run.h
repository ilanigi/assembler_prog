int sec_run(label_table_line *** label_table, int * label_table_size,memory_table_line *** data_table,int * data_table_size, memory_table_line*** instruction_table,int * instruction_table_size, func_node** hash_func_table, label_table_line*** entry_table, int * entry_table_size,int * entry_table_true_size,label_table_line*** extern_table, int * extern_table_size,int * extern_table_true_size, int ICF,char * file_name);

int get_instruction_dst(memory_table_line ** table,int current_index);
