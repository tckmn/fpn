#include <stdlib.h>
#include <stdio.h>

#include "ops.h"
#include "util.h"

// utility functions used in operator implementations

void fpn_push(struct fpn *fpn, void *data, char type) {
    if (fpn->stackSize == fpn->bufSize) {
        fpn->stack = realloc(fpn->stack,
                (fpn->bufSize += FPN_MIN_EXTRA) * sizeof *fpn->stack);
    }
    fpn->stack[fpn->stackSize].data = data;
    fpn->stack[fpn->stackSize].type = type;
    ++fpn->stackSize;
}

void fpn_push_q(struct fpn *fpn, mpq_t q) { fpn_push(fpn, q, RATIONAL); }
void fpn_push_f(struct fpn *fpn, mpfr_t f) { fpn_push(fpn, f, FLOAT); }

void fpn_pop(struct fpn *fpn) {
    fpn_clear(fpn->stack[--fpn->stackSize]);
    if (fpn->bufSize - fpn->stackSize > FPN_MAX_EXTRA) {
        fpn->stack = realloc(fpn->stack,
                (fpn->stackSize + FPN_MIN_EXTRA) * sizeof *fpn->stack);
    }
}

// operators begin here

void fpn_op_add(struct fpn *fpn) {
    CHECK(2, "+");
    BIND(ARG2, a) {
        BIND(ARG1, b) {
            mpq_add(a, a, b);
        } OR {
            mpfr_add_q(b, b, a, fpn->round);
            struct val tmp = ARG2;
            ARG2 = ARG1;
            ARG1 = tmp;
        }
    } OR {
        BIND(ARG1, b) {
            mpfr_add_q(a, a, b, fpn->round);
        } OR {
            mpfr_add(a, a, b, fpn->round);
        }
    }
    fpn_pop(fpn);
}

void fpn_op_print(struct fpn *fpn) {
    CHECK(1, "p");
    BIND(ARG1, val) {
        mpq_out_str(stdout, 10, val);
    } OR {
        mpfr_out_str(stdout, 10, 0, val, fpn->round);
    }
    puts("");
}

char* fpn_op_const(struct fpn *fpn, char *code) {
    int isFloat = 0;
    char *end, old;
    for (end = code; *end; ++end) {
        if (*end == '.') isFloat = 1;
        else if (*end < '0' || '9' < *end) break;
    }
    old = *end;
    *end = 0;

    if (isFloat) {
        mpfr_ptr f = malloc(sizeof *f);
        mpfr_init(f);
        mpfr_set_str(f, code, 10, fpn->round); // TODO assert?
        fpn_push_f(fpn, f);
    } else {
        mpq_ptr q = malloc(sizeof *q);
        mpq_init(q);
        mpq_set_str(q, code, 10); // TODO assert?
        fpn_push_q(fpn, q);
    }

    *end = old;
    return end;
}
