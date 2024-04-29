#include "func.h"
#include<stdbool.h>
#include<stdint.h>

void print_err(int error_code) {
    printf("Error code: %d\n", error_code);
};

bool fclampb(float a, float min, float max) {
  return ((a >= min) && (a <= max));
}

float fclamp(float a, float min, float max)
{
    if (a<min) {
        return min;
    }
    else if (a>max) {
        return max;
    }
    else {
        return a;
    };
};

uint8_t uint8_tclamp(uint8_t a, uint8_t min, uint8_t max)
{
    if (a<min) {
        return min;
    }
    else if (a>max) {
        return max;
    }
    else {
        return a;
    };    
}
