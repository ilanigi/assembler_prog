
void full_clean(label_table_line **label_table,label_table_line** extern_table, label_table_line** entry_table, memory_table_line** instruction_table,memory_table_line** data_table, func_node** hash_func_table, node** label_name_hash, int* label_table_size, int* data_table_size, int* instruction_table_size, int* extern_table_size,int* entry_table_size);

void semi_clean(node ** label_name_hash, int* label_table_size, int* data_table_size, int* instruction_table_size, int* extern_table_size, int* entry_table_size);
