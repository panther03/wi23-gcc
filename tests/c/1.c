int myint;
short myshort;
double mydouble;

void __mulsi3() {
  return;
}

int sumUpTo(int n) {
  int i = 0;
  int sum = 0;
  int thingy;
  while (i < n) {
    sum += i;
    i++;
  }
  thingy = veryBad(sum);
  return i + thingy;
}

int veryBad(int n) {
  if (n <= 0) {
    return 1;
  } else {
    return n + veryBad(n-1);
  }
}

int foo(int x[], int y) {
    return x[1] + x[2] + x[3] + y + sumUpTo (x[3]-y);
}

int g = 0;

void bar(int x) {
  g += x + x + x;
}
int main()
{
  int x[] = {1,2,3};
  int y = 4;
  sumUpTo(x[1]);
  x[2] = foo(x, y);
  return x[2] + x[0] << y;
}