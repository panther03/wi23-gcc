__attribute__((section(".text"))) 
int main() { 
    int x = 0;
    int y = 5;

/*
    y = 5;
    x = 4 / y;
    
    y = 4;
    x = 5 / y;

    y = 4;
    x = 5 % y;

    y = 10;
    x = 4 % y;

    y = 4;
    x = 10 / y;

    y = 27;
    x = 60 % y;

    y = 27;
    x = 60 / y;
*/

    y = 10;
    x = 6094 % y;

//    y = 27;
//    x = 60 / y;

    return 0;
}