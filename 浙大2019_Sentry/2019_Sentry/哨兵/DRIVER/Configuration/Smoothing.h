#ifndef __SMOOTHING_H__
#define __SMOOTHING_H__
#include "struct.h"
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num[1]);
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input);
#endif
