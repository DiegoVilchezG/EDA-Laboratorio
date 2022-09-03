namespace MyNS
{
  int gnCount;
  double Addition(double a, double b) 
  { 
    return a+b;
  }

  long factorial(int n){
    long res = 1;
    for (int i = 2; i <= n; i++){
      res *= i;
    }
    return res;
  }
};
