#include<iostream>
using namespace std;

class ThreeDShape
{
public:
    virtual void display()=0;
    virtual double volume()=0;
};

class Cube : public ThreeDShape
{
private:
    double length,width,height;

public:
    Cube(double a,double b,double c):length(a),width(b),height(c){}

    void setCube(int a,int b,int c)
    {
        length=a;
        width=b;
        height=c;
    }

    double volume()override
    {
        cout<<length*width*height<<endl;
        return length*width*height;
    }

    void display()override
    {
        cout<<length<<endl;
        cout<<width<<endl;
        cout<<height<<endl;
    }
};

int main()
{
    Cube a1(3,4,5);
    ThreeDShape *p;
    p=&a1;
    p->display();
    a1.setCube(2,3,4);
    p->display();
    p->volume();

    return 0;
}
