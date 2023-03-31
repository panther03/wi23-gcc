int myint;
short myshort;
double mydouble;

int sumUpTo(int n) {
  int i = 0;
  int sum = 0;
  while (i < n) {
    sum += i;
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
  int y = 4;
  return foo (x, 222) + x[0] << y;
}