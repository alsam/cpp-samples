// stolen from here [Qt - custom decimal point and thousand separator](https://stackoverflow.com/questions/5035356/qt-custom-decimal-point-and-thousand-separator)
//
//

#include <locale>
#include <sstream>
#include <iostream>

class my_punct
    : public std::numpunct<char>
{
protected:
    virtual char do_decimal_point() const {return ',';}
    virtual char do_thousands_sep() const {return '.';}
    virtual std::string do_grouping() const  {return std::string("\2\3");}
};

int main()
{
    std::ostringstream os;
    os.imbue(std::locale(os.getloc(), new my_punct));
    os.precision(2);
    fixed(os);
    double x = 123456789.12;
    os << x;
    std::string s = os.str();
    std::cout << s << '\n';
}

