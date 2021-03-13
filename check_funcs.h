
int is_legal_reg(char *reg_name);

int is_data(int label_flag,char ** slot_array);

int is_string(int label_flag,char ** slot_array);

int is_valid_data(char ** slot_array,int label_flag, int argument_num);

int is_valid_string(char ** slot_array,int label_flag, int argument_num);

int is_entry(char ** slot_array,int label_flag, int argument_num);

int is_extern(char ** slot_array,int label_flag, int argument_num);

int is_valid_label(char * label, node** hash_saved_words, node** hash_lable_names);

int is_valid_imd(char * imd_val);

