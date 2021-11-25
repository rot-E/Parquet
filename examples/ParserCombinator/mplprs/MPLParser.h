#pragma once

#include <Parquet.h>

typedef struct {
	SeqAnswer_t (* Parser_Program)(List_t *seq, Processor_t *p);
} _MPLParser;

extern _MPLParser MPLParser;
