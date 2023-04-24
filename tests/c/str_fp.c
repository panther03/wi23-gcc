#define STR_FP
#ifdef STR_FP
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
#endif

int check_end(char c) {
    if (c == '\0')
        return 1;
    if (c == ' ')
        return 1; 
    if (c == '+')
        return 1;
    if (c == '-')
        return 1;
    if (c == '*')
        return 1;
    if (c == '/')
        return 1;
    if (c == '(')
        return 1;
    if (c == ')')
        return 1;
    return 0;
}

float pow_10(int pow) {
    float mult = 1.0f;

    if (pow > 0) {
        while (pow > 0) {
            mult *= 10.0f;
            pow -= 1; 
        }
    }
    if (pow < 0) {
        while (pow < 0) {
            mult /= 10.0f;
            pow += 1; 
        }
    }

    return mult;
}

void decode_str(char* str, int* length, int* decimalIndx, int* sign, int* exponent, int* exponentLength, int* err) {

    if (*str == '-') {
        *sign = -1;
	    str++;
    }
    
    while (!check_end(*str)) {
        if (*str == '.') {
            *decimalIndx = *length;
        } else if (*str == 'e') {
            int expLength = -1;
            int expSign = 1;
            char *exp;

	        str++;

            if (*str == '-') {
                expSign = -1;
                str++;
            }

	        exp = str;

            while (!check_end(*str)) {
                expLength++;
		        str++;
            }

	        *exponentLength = expLength;

            while (!check_end(*exp)) {
                if (*exp < '0' || *exp > '9') {
                    *err = 1;
                }

                *exponent += (int)(*exp - '0') * pow_10(expLength);
		        expLength--;
                exp++;
            }
	        *exponent *= expSign;
	        return;
	    } else if (*str < '0' || *str > '9') {
            *err = 1;
        }

        (*length)++;
        str++;
    }
}

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

volatile char* SIM_PRINT_LOC = 0xF000;

void printi(unsigned int integer) {
    write_integer(integer, SIM_PRINT_LOC);
    asm volatile (
        "irq 0"
    );
}

float str_to_fp(char* str, int* err) {
    float fp = 0;
    int pow = 0;
    int decimalIndx = -1; //for default
    int length = 0;
    int sign = 1;
    int exponent = 0;
    int exponentLength = 0;

    decode_str(str, &length, &decimalIndx, &sign, &exponent, &exponentLength, err);


    length -= exponentLength;

    if (decimalIndx == -1) {
        pow = length - 1;
    }
    else {
        pow = decimalIndx - 1;
    }

    int i;
    int digit;
    float mult;
    char curr;
    
    if (sign < 0) {
        str++;
    }
    for (i = 0; i <= length + 1 && !check_end(*(str + i)); i++) {
        curr = *(str + i);

        if (curr == '.') {
            continue;
        }
        else if (curr == 'e') {
            fp *= pow_10(exponent);
            break;
        }

        digit = (int)(curr - '0');
        mult = pow_10(pow);

        fp += digit * mult;
        pow--;
    }

    return fp * sign;
}


__attribute__((section(".text"))) 
int main() {

    const char* str = "-2.54334e34";
    int err = 0;
    float f = str_to_fp(str, &err);

    fp_to_str(f, SIM_PRINT_LOC);

    asm volatile (
        "irq 0"
    );

    return 0;
}