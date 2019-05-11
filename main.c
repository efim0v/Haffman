#include "secondary_functions.h"

void push(Stat_and_TN **mass, int *amount_of_elements, int current_symbol){ ///Функция создает листья для дерева Хаффмана
    if (current_symbol != -1) {
        amount_of_elements[0] += 1;
        if (mass[current_symbol] == NULL) {
            mass[current_symbol] = (Stat_and_TN *) calloc(1, sizeof(Stat_and_TN));
            mass[current_symbol]->symb = current_symbol;
            mass[current_symbol]->value = 1;
            amount_of_elements[1] += 1;
        } else mass[current_symbol]->value++; /// Если символ уже встречался, то его вес увеличивается
    }
}

int determ_val(const Stat_and_TN *cell){ /// Возвращает вес символа (для сортировки)
    if (cell) return (cell) -> value;
    else return 0;
}

int compare(const void *cell_1, const void *cell_2){ /// Возвращает число (если < 0, то первый символ встречается реже второго...)
    return determ_val(cell_1) - determ_val(cell_2);
}

void swap(int a, int b, Stat_and_TN **turn){
    Stat_and_TN *buf = turn[b];
    turn[b] = turn[a];
    turn[a] = buf;
}

void bubble_sort(Stat_and_TN **turn, int unsort_element, int mark){
    while(mark == UNSORTED){
        mark = SORTED;
        for(int i = 1; i <= unsort_element; i++){
            if(compare(turn[i-1], turn[i]) > 0){
                swap(i-1, i, turn);
                mark = UNSORTED;
            }
        }
    }
}

void converter_to_binary_str(int universal_type, int number_of_bits, char *binary_character) {
    for (int i = 0; i < number_of_bits; i++){
        binary_character[i] = (char)((universal_type / (1 << (number_of_bits - i - 1))) + '0');
        universal_type %= 1 << (number_of_bits - i - 1);
    }
}

int converter_to_universal_type(const char *bit_sym, int number_of_bits){
    int summ_bit = 0;
    for (int i = 0; i < number_of_bits; i++){
        if (bit_sym[i] == '\0') break;
        summ_bit += (bit_sym[i] - '0') * (1 << (number_of_bits - i - 1));
    }
    return summ_bit;
}

Stat_and_TN **read_and_count(FILE *in_file, int *amount_of_elements){ /// Функция чтения и подсчета символов входного файла
    Stat_and_TN **turn = (Stat_and_TN **)calloc(256, sizeof(Stat_and_TN*));

    int current_symbol;
    while (!feof(in_file)) {
        current_symbol = fgetc(in_file);
        push(turn, amount_of_elements, current_symbol);
    }
    return turn;
}

Stat_and_TN *node_creation(const char* bit_sym){
    Stat_and_TN *node = (Stat_and_TN *)calloc(1, sizeof(Stat_and_TN));
    if (bit_sym != NULL) node -> symb = converter_to_universal_type(bit_sym, CHAR);
    else node -> symb = NSYMB;
    return node;
}

Stat_and_TN *tree_building(Stat_and_TN **turn, const int *amount_of_elements){
    int correction;
    if(amount_of_elements[1] == 1){
        correction = 1;
        bubble_sort(turn, 255, UNSORTED);
        turn[254] = calloc(1, sizeof(Stat_and_TN));
        turn[254] -> symb = 2;
    }
    else correction = 0;

    for (int i = (256 - amount_of_elements[1] - correction); i < 255; i++){

        bubble_sort(turn, 255, UNSORTED);

        Stat_and_TN *root = node_creation(NULL);
        root->value = determ_val(turn[i]) + determ_val(turn[i + 1]);
        root->right = turn[i + 1 - correction];
        root->left = turn[i + correction];

        turn[i + 1] = root;
        turn[i] = NULL;
    }
    return turn[255];
}

void fill_codes(Stat_and_TN *root, char buffer[256], char **t_code){
    if (root->symb != NSYMB){
        t_code[root->symb] = calloc(256, sizeof(char));
        strcpy(t_code[root->symb], buffer);
        return;
    }
    char lcode[256] = {'\0'};
    char rcode[256] = {'\0'};

    strcpy(lcode, buffer);
    lcode[strlen(buffer)] = '0';

    strcpy(rcode, buffer);
    rcode[strlen(buffer)] = '1';

    if (root -> left) fill_codes(root->left, lcode, t_code);
    if (root -> right) fill_codes(root->right, rcode, t_code);
}

void write_bit(char bin_code_symbol, write_struct *global, int parametr, FILE *out_file){
    if (parametr == WRITE){
        global->binary_write_buffer[global->current_position] = bin_code_symbol;
        global->current_position++;
        if (global->current_position == 8)
            write_bit('\0', global, UNLOAD, out_file);
    }
    if (parametr == UNLOAD){
        putc((unsigned char)(converter_to_universal_type(global->binary_write_buffer, CHAR)), out_file);

        memset(global->binary_write_buffer, '\0', sizeof(char)*8);
        global->current_position = 0;
    }
}

void tree_entry(Stat_and_TN *root, FILE *out_file, write_struct *global, char *binary_character){
    if ((root->left != NULL) && (root->right != NULL)) { ///если не лист
        write_bit('0', global, WRITE, out_file);
        tree_entry(root->left, out_file, global, binary_character);
        tree_entry(root->right, out_file, global, binary_character);
    }else{
        write_bit('1', global, WRITE, out_file);
        converter_to_binary_str(root->symb, CHAR, binary_character);
        for (int i = 0; i < 8; i++)
            write_bit(binary_character[i], global, WRITE, out_file);
        memset(binary_character, '\0', sizeof(char)*9);
    }
}

void encoder(FILE *in_file, FILE *out_file, char **t_code, write_struct *global){
    int helper;
    while (!feof(in_file)){
        if ((helper = fgetc(in_file)) != -1) {
            for (int i = 0; i < strlen(t_code[helper]); i++) write_bit(t_code[helper][i], global, WRITE, out_file);
        }
    }
    write_bit('\0', global, UNLOAD, out_file);
}

void int_entry(int number, FILE *out_file){
    char int_in_birary_form[34] = {'\0'};
    converter_to_binary_str(number, 32, int_in_birary_form);
    for (int i = 0; i < 4; i++) putc((unsigned char)(converter_to_universal_type(&int_in_birary_form[i * 8], CHAR)), out_file);
}

void compression(FILE *in_file, FILE *out_file, FILE *cheat){

    int *amount_of_elements = (int *)(malloc(sizeof(int)*2));
    amount_of_elements[0] = 0; /// Количество символов в исходном тексте
    amount_of_elements[1] = 0; /// Количество различный символов

    Stat_and_TN **turn = read_and_count(in_file, amount_of_elements);
    Stat_and_TN *root = tree_building(turn, amount_of_elements);


    enter_to_gv_file("graph_c.gv", root);

    if (root ->value != amount_of_elements[0]) exit(EXIT_FAILURE); ///Проверка дерева на включение всех символов

    char **code_table = (char **)calloc(256*256, sizeof(char*));
    char *buffer = (char *)calloc(256, sizeof(char));

    fill_codes(root, buffer, code_table);

    write_struct *global = (write_struct *)(malloc(sizeof(write_struct))); ///Подумать над названием

    memset(buffer, '\0', sizeof(*buffer));
    global->binary_write_buffer = buffer;
    global->current_position = 0;

    char *binary_character = (char *)(calloc(9, sizeof(char)));

    int_entry(amount_of_elements[0], cheat);

    tree_entry(root, out_file, global, binary_character);
    write_bit('\0', global, UNLOAD, out_file);
    free(binary_character);


    fseek(in_file, 3, SEEK_SET);
    encoder(in_file, out_file, code_table, global);


    free(amount_of_elements);
    free(turn);
    free(code_table);
    free(buffer);
}

char *buf_filling(const int *counter, char *bit_str, FILE *in_file){
    char *buffer = calloc(9, sizeof(char));
    for (int i = 0; i < 8; i++){
        if (bit_str[*counter + i] == '\0') converter_to_binary_str(fgetc(in_file), CHAR, &bit_str[*counter + i]);
        buffer[i] = bit_str[*counter + i];
    }
    return buffer;
}

Stat_and_TN *decompression_tree_building(char *bit_str, int *counter, FILE *in_file){
    if (bit_str[*counter] == '\0') converter_to_binary_str(fgetc(in_file), CHAR, &bit_str[*counter]);
    if (bit_str[*counter] == '1'){
        *counter += 1;
        Stat_and_TN *node = node_creation(buf_filling(counter, bit_str, in_file));
        *counter += 8;
        return node;
    } else {
        Stat_and_TN *node = node_creation(NULL);
        *counter += 1;
        node->left = decompression_tree_building(bit_str, counter, in_file);
        node->right = decompression_tree_building(bit_str, counter, in_file);
        return node;
    }
}

void last_pass(FILE *in_file, FILE *out_file, Stat_and_TN *root, char *bit_sym, int symb_numbers) {

    int helper = 0;
    int counter = 0;
    Stat_and_TN *node = root;
    while (!feof(in_file)) {
        helper = fgetc(in_file);
        converter_to_binary_str(helper, CHAR, bit_sym);
        for (int i = 0; i < 8; i++) {
            if ((node->left) && (node->right)) {
                if (bit_sym[i] == '0') node = node->left;
                else if (bit_sym[i] == '1') node = node->right;
            } else {
                if (counter == symb_numbers) break;
                putc(node->symb, out_file);
                counter++;
                if (bit_sym[i] == '0') node = root->left;
                else if (bit_sym[i] == '1')
                    node = root->right;
            }
        }
    }
}

void decompression(FILE *in_file, FILE *out_file, FILE *chaet){

    char *bit_str = (char *)calloc(4096, sizeof(char));
    char *bit_sym = (char *)calloc(9, sizeof(char));

    for(int i = 0; i < 4; i++) converter_to_binary_str(fgetc(chaet), CHAR, &bit_str[i*8]);
    int symb_numbers = converter_to_universal_type(bit_str, 32);
    memset(bit_str, '\0', sizeof(char)*4096);

    int *counter = (int *)calloc(1, sizeof(int));
    Stat_and_TN *root = decompression_tree_building(bit_str, counter, in_file);

    enter_to_gv_file("graph_d.gv", root);

    last_pass(in_file, out_file, root, bit_sym, symb_numbers);
    free(bit_str);
    free(bit_sym);
}

int main() {

    FILE *in_file;
    in_file = fopen("in.txt", "rb");

    FILE *out_file;
    out_file = fopen("out.txt", "wb");
    if (in_file == NULL) exit(EXIT_FAILURE);
    if (out_file == NULL) exit(EXIT_FAILURE);

    int argument = fgetc(in_file);
    if(argument == -1) exit(EXIT_FAILURE);

    fseek(in_file, 2, SEEK_CUR);
    if(fgetc(in_file) == -1) exit(EXIT_SUCCESS);
    fseek(in_file, -1, SEEK_CUR);

    FILE *cheat;
    if (argument == 'c') cheat = fopen("cheat.txt", "wb");
    else cheat = fopen("cheat.txt", "rb");


    if (argument == 'c') compression(in_file, out_file, cheat);
    if (argument == 'd') decompression(in_file, out_file, cheat);

    if (fclose(in_file) == EOF) exit(EXIT_FAILURE);
    if (fclose(out_file) == EOF) exit(EXIT_FAILURE);
    if (fclose(cheat) == EOF) exit(EXIT_FAILURE);
    return 0;
}