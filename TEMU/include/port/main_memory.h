//
// Created by hanyuan on 2024/5/6.
//

#ifndef TEMU_MAIN_MEMORY_H
#define TEMU_MAIN_MEMORY_H

#include <stdint.h>
#include "parameters.h"

void port_main_memory_read_word_batch(uint32_t offset, uint32_t *readBuffer, uint32_t batchSize);

void port_main_memory_write_word_batch(uint32_t offset, uint32_t *wordData, uint32_t batchSize);

void port_main_memory_load_byte_batch(uint32_t offset, uint8_t *byteData, uint32_t batchSize);

#endif //TEMU_MAIN_MEMORY_H
