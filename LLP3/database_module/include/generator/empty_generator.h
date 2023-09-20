#ifndef EMPTY_GENERATOR_H
#define EMPTY_GENERATOR_H

#include <stdlib.h>
#include "adv_types/tree_header_tools.h"
#include "basic_types/tree_header.h"
/**
 * Генерирует пустой заголовок для файла
 * @param pattern шаблон вершин
 * @param types типы для шаблона
 * @param pattern_size размер массива шаблона
 * @param key_sizes длины строк шаблона
 * @param header контейнер для заголовка
 */
struct tree_header;

void generate_empty_tree_header(char **pattern, uint32_t *types,size_t pattern_size, size_t *key_sizes, struct tree_header *header);

#endif
