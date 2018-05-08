#include "util.h"

int int_pow(int x, int y) {
	// uses exponentiation by squaring method
    if(y == 0) {
        return 1;
    }
    int root = int_pow(x, y/2);
    if (y % 2 == 0) {
        return root * root;
    }
    else {
        return x * root * root;
    }
}
