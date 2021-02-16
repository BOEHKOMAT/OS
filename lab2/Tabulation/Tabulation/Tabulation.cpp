#include <iostream>
#include <math.h>
using namespace std;

int main()
{
	double x = 1, arctg = 0, y = 0;
	int A = 19, B = 171215;
	for (int n = A; n < B; n++)
	{
		y = pow(-1, n) * (pow(x, 2 * n + 1) / (2 * n + 1));
		cout << y << endl;
		arctg += y;
	}
	cout << arctg << endl;
	return 0;
}