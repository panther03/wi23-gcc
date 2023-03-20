int myint;
short myshort;
double mydouble;

int test() {
  int i = 0;
  if (i == 0) {
    i++;
  }
  return i;
}

int foo(int x[], int y) {
    return x[1] + x[2] + x[3] + y + sumUpTo (x[3]-y);
}
int main()
{
  int x[] = {1,2,3};
  return foo (x, 222);
}