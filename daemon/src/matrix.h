#ifndef MATRIX_H
#define MATRIX_H

#include "matrix_ll.h"

void matrix_init(void);
void matrix_close(void);

void matrix_update(ll_picture_t *picture);
void matrix_setFrame(ll_frame_t *frame);

void *matrix_run(void*);

#endif /* MATRIX_H */
