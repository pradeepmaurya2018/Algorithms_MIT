/*
 * Expression optimizer
 * --------------------
 * Imagine you are writing a compiler and an expression like "1 + 5" has been
 * parsed. Compilers optimize expressions to produce faster or smaller machine
 * instruction sequences. The expression "1 + 5" consists of constants and can
 * be optimized to "6" at compile-time. This is called constant folding.
 *
 * Your task is to implement the expr_optimize() function to perform constant
 * folding:
 *
 *   Expr *expr = expr_add(
 *       expr_literal(1),
 *       expr_literal(5),
 *   );
 *
 *   expr_optimize(expr);
 *
 * After expr_optimize(expr), the expression tree has been replaced by a single
 * expr_literal(6). Larger input expressions are also possible since expr_add()
 * can be nested and should be optimized as well.
 *
 * Fill in the missing function bodies in this source file so that the test
 * cases in main() pass.
 */

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    OP_LITERAL, /* a signed integer constant */
    OP_ADD,     /* arithmetic addition of left and right children */
} Op;

/* An expression tree node */
typedef struct Expr Expr;
struct Expr {
    Op op;
    union {
        /* OP_LITERAL */
        struct {
            int value; /* the signed integer constant */
        } literal;

        /* OP_ADD */
        struct {
            Expr *left;  /* the left child node */
            Expr *right; /* the right child node */
        } add;
    };
};

/* Create a new OP_LITERAL expression */
static Expr *expr_literal(int value)
{
    Expr *expr = (Expr*)malloc(sizeof(*expr));
    if (expr == NULL) {
        fputs("expr_literal malloc failed", stderr);
        exit(EXIT_FAILURE);
    }

    expr->op= OP_LITERAL;
    expr->literal.value = value;

    return expr;
}

/* Create a new OP_ADD expression */
static Expr *expr_add(Expr *left, Expr *right)
{
    Expr *expr = (Expr*) malloc(sizeof(*expr));
    if (expr == NULL) {
        fputs("expr_literal malloc failed", stderr);
        exit(EXIT_FAILURE);
    }

    expr->op = OP_ADD;
    // expr->literal.value =0;
    expr->add.left=left;
    expr->add.right=right;

    return expr;
    /* TODO */
    // struct Expr* result = expr_literal(left->literal.value+ right->literal.value);

}

/* Destroy an expression tree by freeing the node and its children */
static void expr_free(Expr *expr)
{
    /* TODO */
}

/*
 * Apply constant folding to the expression by replacing OP_ADD nodes with
 * OP_LITERAL nodes when the addition can be performed at compile-time.
 * Modifies expr and frees any old nodes that are no longer needed.
 */
static void expr_optimize(Expr *expr)
{
    /* TODO */
    if(expr->op==OP_LITERAL) return;
    if(expr->op==OP_ADD) {
        expr_optimize(expr->add.left);
        expr_optimize(expr->add.right);
    }
    expr->op=OP_LITERAL;
    int result=expr->add.left->literal.value+ expr->add.right->literal.value;
    free(expr->add.left);
    free(expr->add.right);
    expr->literal.value=0;
    expr->literal.value=result;


}

/* Test cases. You do not need to read the code below. */

static void indent(unsigned level)
{
    if (level > 0) {
        fprintf(stderr, "%*c", level * 2, ' ');
    }
}

static void expr_dump(Expr *expr, unsigned indent_level, const char *terminator)
{
    indent(indent_level);

    if (expr == NULL) {
        fprintf(stderr, "NULL%s", terminator);
        return;
    }

    switch (expr->op) {
    case OP_LITERAL:
        fprintf(stderr, "expr_literal(%d)%s", expr->literal.value, terminator);
        break;

    case OP_ADD:
        fprintf(stderr, "expr_add(\n");
        expr_dump(expr->add.left, indent_level + 1, ",\n");
        expr_dump(expr->add.right, indent_level + 1, "\n");
        indent(indent_level);
        fprintf(stderr, ")%s", terminator);
        break;
    }
}

static void test(Expr *input, int expected)
{
    expr_optimize(input);

    if (input->op != OP_LITERAL) {
        fprintf(stderr, "Expr not optimized to a literal:\n");
        expr_dump(input, 0, "\n");
        exit(EXIT_FAILURE);
    }

    if (input->literal.value != expected) {
        fprintf(stderr, "Expected literal value %d, got %d:\n",
                expected, input->literal.value);
        expr_dump(input, 0, "\n");
        exit(EXIT_FAILURE);
    }
    printf("Success \n");
    expr_free(input);
}

int main(int argc, char **argv)
{
    test(
        expr_literal(123),
        123
    );

    test(
        expr_add(expr_literal(2), expr_literal(2)),
        4
    );

    test(
        expr_add(expr_literal(3), expr_literal(-4)),
        -1
    );
    //
    test(
        expr_add(
            expr_add(
                expr_literal(8),
                expr_literal(2)
            ),
            expr_literal(5)
        ),
        15
    );
    //
    test(
        expr_add(
            expr_literal(2),
            expr_add(
                expr_literal(7),
                expr_literal(3)
            )
        ),
        12
    );

    test(
        expr_add(
            expr_add(
                expr_literal(4),
                expr_literal(8)
            ),
            expr_add(
                expr_literal(9),
                expr_literal(2)
            )
        ),
        23
    );

    printf("OK\n");
    return EXIT_SUCCESS;
}