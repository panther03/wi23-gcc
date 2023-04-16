int myint;
short myshort;
double mydouble;

int foo(int x[], int y) {
    if (x[3] > 69) {
      return 420;
    } else {
      return 0;
    }
    //return x[1] + x[2] + x[3] + y + sumUpTo (x[3]-y);
}

int g = 0;

void bar(int x) {
  g += x + x + x;
}
int main()
{
  int x[] = {1,2,3};
  int y = 4;
  //sumUpTo(x[1]);
  x[2] = foo(x, y);
  return x[2] + x[0] << y;
}