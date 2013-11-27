#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

class Matrix{
	friend ostream& operator<<(ostream& o, const Matrix& a);
	public:
	Matrix();
	Matrix(int r, int c);	
	void init(int a);	//初始化矩阵
	Matrix(const Matrix& a);
	~Matrix();
	Matrix& operator=(const Matrix & a);
	Matrix operator+(const Matrix & a);
	Matrix operator*(const Matrix & a);
	Matrix transpose();
	int & get_value(int r, int c);

	private:
	int **value;//数值
	int row;//行
	int col;//列
};

Matrix::Matrix()
:row(1),col(1)
{
	value = new int *[row]; 
	for (int i = 0; i < row; i++)
		value[i] = new int[col];
}

void Matrix::init(int a)
{
	int i, j;
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++ )
			value[i][j] = a++;
}

Matrix::Matrix(int r, int c)
:row(r),col(c)
{
	value = new int *[r];
	int i, j;
	for (i = 0; i < r; i++)
	{
		value[i] = new int [c];
		//for (j = 0; j < c; j++)
		//	value[i][j] = 0;
	}
}

Matrix::Matrix(const Matrix& a)
:row(a.row), col(a.col)
{
	int i, j;
	value = new int*[row];
	for (i = 0; i < row; i++)
	{
		value[i] = a.value[i];
		for (j = 0; j < col; j++)
			value[i][j] = a.value[i][j];
	}
}

Matrix::~Matrix()
{
	int i;
	for (i = 0; i < row; i++)
		delete[] value[i];
	delete[] value;
}

ostream& operator<<(ostream& o, const Matrix & a)
{
	int i, j;
	o << right;	//右对齐
	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
			o <<  setw(4) << a.value[i][j];	//设置字符显示宽度为4
		//setw每次设置后会自动变为setw(0)，所以它只生效一次,每次显示数值都要重新设置
		o << endl;
	}
	o << "======================" << endl;
	return o;
}
Matrix& Matrix::operator=(const Matrix & a)
{
	if (this == &a)
		return *this;

	for (int i = 0; i < row; i++)
		delete[] value[i];
	delete[] value;

	int i, j;
	row = a.row;
	col = a.col;
	value = new int*[row];
	for (i = 0; i < row; i++)
	{
		value[i] = new int [col];
		for (j = 0; j < col; j++)
			value[i][j] = a.value[i][j];
	}
	return *this;
}

Matrix Matrix::operator+(const Matrix& a)
{
	if ((row != a.row) || (col != a.col))
		throw 1;	//抛出一个异常

	Matrix temp(row, col);
	int i, j;
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			temp.value[i][j] = value[i][j] + a.value[i][j];
	return temp;
}

Matrix Matrix::operator*(const Matrix& a)
{
	if (col != a.row)
		throw 2;

	Matrix temp(row, a.col);
	int i, j;
		for (i = 0; i < row; i++)
			for (j = 0; j < a.col; j++)
		{
			int sum = 0;
				for(int k = 0; k < a.row; k++)
				 sum += value[i][k] * a.value[k][j];
					temp.value[i][j] = sum;
		}
		return temp;
}

Matrix Matrix::transpose()
{
	int i, j;
	Matrix temp(col,row);
	for (i = 0; i < temp.row; i++)
		for (j = 0; j < temp.col; j++)
			temp.value[i][j] = value[j][i];
			return temp;
}

int& Matrix::get_value(int r, int c)
{
	if ((r < 1) || (r > row) || (c < 1) || (c > col))
		throw 3;

		return value[r - 1][c - 1];
}

int main()
{
	Matrix c;
	c.init(8);

	Matrix a(3,3);
	a.init(1);

	Matrix b(2,2);
	b.init(2);

	cout << a << endl;
	cout << b << endl;
	try{
		c = a * b;
		cout << c;
	a.get_value(4,3) = 100;
	cout << a << endl;
	}
	catch(int e)
	{
		if(e == 1)
			cerr << "can't add" << endl;
		else if (e == 2)
			cerr << "can't mul" << endl;
		 else if (e == 3)
			cerr << "can't get_value" << endl;
	}
	c = a;
	cout << c << endl;
#if 0
	cout << "here it is" << endl;
	Matrix f(3,7);
	f.init(6);
	c = f.transpose();
	cout << c;
//	cout << a * b << endl;
	cout << "==== operator= =======" << endl;
	c = a;
	cout << c << endl;
	cout << "========== Matrix(const Matrix& a) =======" << endl;
	Matrix d(b);
	cout << d << endl;
#endif

	return 0;
}
