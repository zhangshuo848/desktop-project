//设计一个MyVector（矢量）类，在类中定义整型成员变量x，y，z代表矢量在三维笛卡尔坐标系上的坐标；成员函数包含构造函数、返回坐标值的函数getX(), getY()和getZ(),
//以及计算模长（设x, y, z为其三维坐标，则模长为sqrt(x*x+y*y+z*z）的函数getLength（），返回模长值（浮点型）
#include<iostream>
#include<math.h>
using namespace std;

class MyVector
{
private:
    int x,y,z;
    
public:

    MyVector(int a,int b,int c):x(a),y(b),z(c){}
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    int getZ()
    {
        return z;
    }

    double getLength()
    {
        return sqrt(x*x+y*y+z*z);
    }
};
int main()
{
   MyVector a(1,2,3);
   cout<<a.getLength()<<endl;

   return 0;
}