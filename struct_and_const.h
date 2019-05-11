#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef LABLAB_STRUCT_AND_CONST_H
#define LABLAB_STRUCT_AND_CONST_H

typedef struct Stat_and_TN{
    int value;  /// Вес символа
    int symb;    /// Код символа в ASCLL
    struct Stat_and_TN *left; /// Левый потомок
    struct Stat_and_TN *right; /// Правый потомок
} Stat_and_TN;

typedef struct write_struct{
    char *binary_write_buffer; /// Массив битов
    int current_position; /// Текущая позиция в массиве
} write_struct;

#define NSYMB 1000

/// Константы для функции write_bit

#define WRITE 1100
#define UNLOAD 1200

/// Константы для записи функции сортировки:

#define SORTED 2000
#define UNSORTED 2100

///Константы для записи числа

#define CHAR 8

#define NOT_THE_END 100

#endif //LABLAB_STRUCT_AND_CONST_H