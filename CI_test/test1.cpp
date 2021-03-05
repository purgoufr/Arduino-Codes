#include <iostream>
#include <initializer_list>

using namespace std;

//MANUPULATOR ORNEGI
std::ostream &dline(std::ostream &os)
{
    return os << "-----------------------------\n";
}

//////////////////////////////////////////////
// void func(std::initializer_list<int> x);
//func({2, 4, 6, 8}); // func parameter initializer list olabilir
//////////////////////////////////////////////
class Myclass
{
public:
    Myclass(std::initializer_list<double> dlist)
    {
        double sum{};
        for (auto val:dlist)
            sum +=val;
        mean = sum/dlist.size();
        cout<<"mean is : "<<mean<<"\n";
    }
private:
    double mean;
};
//////////////////////////////////////////////
/*Eger bir class in hem int parametreli hem de initializer list
parametreli ctor'u varsa {} icerisinde cagirirsan initializer list
parametreli ctor'u cagirir*/
class Myclass2
{
public:
    Myclass2(int)
    {
        cout<<"Myclass2(int)\n";
    }
    Myclass2(std::initializer_list<int> x)
    {
        cout<<"Myclass2(std::initializer_list<int>)\n";
    }
};

int main()
{
    std::initializer_list<int> x{1,3,5,6,10}; //std::initializer_list<int> x = {1,3,5,6,10}; ayni

    auto mylist = {12, 34, 89, 90};
    auto mylist2 = {1.2, 3.4, 8.9, 9.0}; 
    auto mylist3 = {'A', 'B', 'C'};

    // auto x{12} ;  <==> int x = 12 ; ile ayni
    // auto x{11,12}; //syntax error
    cout<<dline;
    //////////////////////////////////////////////////
    int x1 = 10;
    int x2 = 11;
    int x3 = 12;

    auto mylist4 = {x1, x2, x3};
    cout<<"begin : "<< mylist4.begin() <<"\tend : "<< mylist4.end() <<"\tsize :"<< mylist4.size()<<"\n";
    cout<<dline;
    //////////////////////////////////////////////////
    Myclass mclass1{2.5, 6.8, 22.7};
    cout<<dline;
    //////////////////////////////////////////////////
    /*range base for loop*/
    for (auto i : {1,2,5,8,9,12})
        cout<<i<<"-";
    cout<<"\n";
    cout<<dline;
    //////////////////////////////////////////////////
    auto mylist5 = {1, 5, 8, 9, 3};

    auto iter = mylist5.begin();
    cout<< *iter << "\n";
    iter++;         // *iter = 5 gecersiz atama yapilamaz!
    cout<< *iter << "\n";
    cout<<dline;
    //////////////////////////////////////////////////
    Myclass2 m1(2);
    Myclass2 m2 = 2;
    Myclass2 m3{2};
    cout<<dline;
    //////////////////////////////////////////////////
    return 0;
}