#include "secondary_functions.h"

void dfs_v(FILE *tFile, Stat_and_TN* root, int counter){
    if (root->left && root->right){
        fprintf(tFile, "v%d [shape=box label=%d];\n", counter, root->value);
        fprintf(tFile, "v%d -> v%d [label=0];\n", counter, 2*counter + 1);
        fprintf(tFile, "v%d -> v%d [label=1];\n", counter, 2*counter + 2);
        dfs_v(tFile, root->left, 2*counter + 1);
        dfs_v(tFile, root->right, 2*counter + 2);
    }
    else{
        if (isprint((char)(root->symb))){
            fprintf(tFile, "v%d [label=\"%c - %d\"];\n", counter, (char)(root->symb), root->value);
        }
        else{
            fprintf(tFile, "v%d [label=\"%d - %d\"];\n", counter, (char)(root->symb), root->value);
        }
    }
}

void enter_to_gv_file(char *name_gv_file, Stat_and_TN *root){
    FILE *graphl;
    graphl = fopen(name_gv_file, "w");
    if (graphl == NULL) exit(EXIT_FAILURE);

    fprintf(graphl, "digraph main{\n");
    dfs_v(graphl, root, 0);
    fprintf(graphl, "}");
}