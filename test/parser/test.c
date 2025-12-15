struct Point {
    int x;
    int y;
};

struct Point global_origin;

int add(int a, int b) {
    int result;
    result = a + b;
    return result;
}

int max_int(int a, int b) {
    if (a < b)
        return b;
    else
        return a;
}

int ternary_demo(int x, int y) {
    int v;
    /* nested ternary, right-associative */
    v = x < 0 ? -x : x < y ? x : y;
    return v;
}

int loop_demo(int n) {
    int i;
    int sum;

    i = 0;
    sum = 0;

start:
    while (i < n) {
        if (i == 3) {
            i = i + 1;
            continue;
        }

        if (i == 7) {
            break;
        }

        sum = sum + i;
        i = i + 1;
    }

    if (n < 0) {
        goto start;
    }

    return sum;
}

/* original sizeof tests + some pointer ones */
int sizeof_demo(struct Point *p) {
    int s1;
    int s2;
    int s3;
    int s4;

    /* sizeof(type-name) */
    s1 = sizeof(int);
    s4 = sizeof(int*);

    /* sizeof(unary-expression) */
    s2 = sizeof *p;
    s3 = sizeof (p->x);

    return s1 + s2 + s3 + s4;
}

/* NEW: explicitly tests abstract-declarator / direct-abstract-declarator
 *
 * C4 grammar:
 *   type-name:
 *       type-specifier abstract-declarator_opt
 *
 *   abstract-declarator:
 *       pointer
 *       pointer_opt direct-abstract-declarator
 *
 *   direct-abstract-declarator:
 *       ( abstract-declarator )
 *       direct-abstract-declarator_opt ( parameter-list )
 */
int sizeof_abstract_demo(void) {
    int s1;
    int s2;
    int s3;

    /* abstract-declarator = pointer */
    s1 = sizeof(int*);     /* int * */

    /* abstract-declarator = pointer (with two stars) */
    s2 = sizeof(int**);    /* int ** */

    /* abstract-declarator = pointer_opt direct-abstract-declarator
       here:  int (*)(int a, int b)
       - pointer_opt:        '*'
       - direct-abstract-declarator:  ( parameter-list )
         where parameter-list is: 'int a, int b'
     */
    s3 = sizeof(int (*)(int a, int b));

    return s1 + s2 + s3;
}

int main() {
    struct Point p;
    int a;
    int b;
    int c;
    int d;

    a = 3;
    b = 4;

    c = add(a, b);

    if (c < 10) {
        c = max_int(c, 10);
    }

    d = ternary_demo(c, b);
    d = d + loop_demo(10);

    d = d + sizeof_demo(&p);

    /* NEW: exercise abstract-declarator parsing */
    d = d + sizeof_abstract_demo();

    return d;
}
