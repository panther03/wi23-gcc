//#include "str_fp.h"

volatile char* SIM_PRINT_LOC = 0xF000;

int write_integer(unsigned int integer, char *buff) {
    char intBuff[16];
    char *indx = intBuff + 15;
    int count = 1;
     
    *indx = (char)(integer % 10) + '0';
    integer = integer / 10;
    while (integer) {
        *--indx = (char)(integer % 10) + '0';
        integer = integer / 10;
        count++;	
    }

    int i;
    for (i = 0; i < count; i++) {
        *buff++ = *(indx + i);
    }

    return count;
}

int write_decimal(unsigned int decimal, char *buff) {
    int width = 6;
    
    while (decimal % 10 == 0 && width > 0) {
        decimal /= 10;
	width--;
    }

    char intBuff[16];
    char *indx = intBuff + 15;
    int count = 1;

    *indx = '\0';
    while (width--) {
        *--indx = (char)(decimal % 10) + '0';
	decimal /= 10;
	count++;
    }
    *--indx = '.';

    int i;
    for (i = 0; i < count; i++) {
        *buff++ = *(indx + i);
    }

    return count;
}

int normalize_float(float *fp) {
    float posExpThresh = 1e7;
    float negExpThresh = 1e-5;
    int exponent = 0;

    if (*fp >= posExpThresh) {
        if (*fp >= 1e32) {
            *fp /= 1e32;
            exponent += 32;
        }
        if (*fp >= 1e16) {
            *fp /= 1e16;
            exponent += 16;
        }
        if (*fp >= 1e8) {
            *fp /= 1e8;
            exponent += 8;
        }
        if (*fp >= 1e4) {
            *fp /= 1e4;
            exponent += 4;
        }
        if (*fp >= 1e4) {
            *fp /= 1e4;
            exponent += 4;
        }
        if (*fp >= 1e2) {
            *fp /= 1e2;
            exponent += 2;
        }
        if (*fp >= 1e1) {
            *fp /= 1e1;
            exponent += 1;
        }
    }

    if (*fp > 0 && *fp <= negExpThresh) {
        if (*fp < 1e-31) {
            *fp *= 1e32f;
            exponent -= 32;
        }
        if (*fp < 1e-15) {
            *fp *= 1e16f;
            exponent -= 16;
        }
        if (*fp < 1e-7) {
            *fp *= 1e8f;
            exponent -= 8;
        }
        if (*fp < 1e-3) {
            *fp *= 1e4f;
            exponent -= 4;
        }
        if (*fp < 1e-1) {
            *fp *= 1e2f;
            exponent -= 2;
        }
        if (*fp < 1e0) {
            *fp *= 1e1f;
            exponent -= 1;
        }
    }

    return exponent;
}

void split_float(float *fp, int *integer, int *decimal, int *exponent) {
    *exponent = normalize_float(fp);

    *integer = (unsigned int)*fp;
    float remainder = *fp - *integer;

    remainder *= 1e6;
    *decimal = (unsigned int)remainder;

    // rounding
    remainder -= *decimal;
    if (remainder >= .5) {
        (*decimal)++;
	if (*decimal >= 1e6) {
            *decimal = 0;
	    (*integer)++;
            if (*exponent != 0 && *integer >= 10) {
                (*exponent)++;
                *integer = 1;
            }
        }
    }
}

void fp_to_str(float fp, char *buff) {
    unsigned int integer, decimal;
    int exponent;
    int integerDigs;
    int decimalDigs;
    int exponentDigs;

    if (fp != fp) { // is nan
        *buff++ = 'n';
	*buff++ = 'a';
	*buff++ = 'n';
	*buff = '\0';
	return;
    }
  
    if (fp < 0.0) {
        *buff++ = '-';
        fp = -fp;	
    }

    if (fp - fp != 0) { // is inf
        *buff++ = 'i';
        *buff++ = 'n';
        *buff++ = 'f';
	*buff = '\0';
	return;
    }

    split_float(&fp, &integer, &decimal, &exponent);
    integerDigs = write_integer(integer, buff);
    buff += integerDigs;

    if (decimal) {
        decimalDigs = write_decimal(decimal, buff);
        buff += decimalDigs;
    }

    if (exponent < 0) {
        *buff++ = 'e';
        *buff++ = '-';
	exponentDigs = write_integer(-exponent, buff);
	buff += exponentDigs;
    }
    else if (exponent > 0) {
        *buff++ = 'e';
        exponentDigs = write_integer(exponent, buff);
	buff += exponentDigs;
    }

    *buff = '\0';
    return;
}

__attribute__((section(".text"))) 
int main() {

    fp_to_str(3.1414f, SIM_PRINT_LOC);

    asm volatile (
        "irq 0"
    );

    return 0;
}