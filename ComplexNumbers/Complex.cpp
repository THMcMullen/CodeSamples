//Assignment 1, 159.732
//Tim McMullen, 06222757

#include <iostream>
#include <math.h>
using namespace std;


//Promote Method, to find what type to work with.
//Taken from http://www.devmaster.net/forums/showthread.php?t=12199
template<bool, typename T, typename U>
struct meta_if{
  typedef T res;
};

template<typename T, typename U>
struct meta_if<false, T, U>{
  typedef U res;
};

template<typename T, typename U>
struct promote{
  typedef typename meta_if<(sizeof(T)>=sizeof(U)), T, U>::res res;
};

//Complex class
template <typename T>
class complex{
	private:
		T real;
		T imag;
	
	public:
		complex(T temp_real = 0, T temp_imag = 0):real(temp_real),imag(temp_imag){}
		friend istream& operator>> (istream& in, complex<T> &cc){
			in >> cc.real >> cc.imag;
			return in;
		}
		friend ostream& operator<< (ostream& out, complex<T> &cc){
			out << cc.real<< " " << cc.imag << "i";
			return out;			
		}
		//complex addition
		template	<typename A, typename B>
		friend complex<typename promote<A,B>::res>& operator+(complex<A>& complexA, complex<B>& complexB); 
		//complex multiplication
		template	<typename A, typename B>
		friend complex<typename promote<A,B>::res>& operator*(complex<A>& complexA, complex<B>& complexB);
		//complex division
		template	<typename A, typename B>
		friend complex<typename promote<A,B>::res>& operator/(complex<A>& complexA, complex<B>& complexB);
		//complex absolute value
		template	<typename A>
		friend A abs(complex<A>& complexA);
		//complex conjugation
		template	<typename A>
		friend complex<A>& conj(complex<A>& complexA);
		//complex normalize
		template	<typename A>
		friend complex<A>& norm(complex<A>& complexA);
		//complex exponential
		template	<typename A>
		friend complex<A>& exp(complex<A>& complexA);
		
};


//complex addition
//formulas from wikipedia, http://en.wikipedia.org/wiki/Complex_number
	template	<typename A, typename B>
	complex<typename promote<A,B>::res>& operator+(complex<A>& complexA, complex<B>& complexB){
		typedef class promote<A,B>::res Type;
		Type real = complexA.real + complexB.real;
		Type imag = complexA.imag + complexB.imag;
		complex<Type> *next = new complex<Type>(real,imag);
		return *next;
	}
	
//complex multiplication
//(a + bi)(c + di) = (ac - bd) + (bc+ad)i
	template	<typename A, typename B>
	complex<typename promote<A,B>::res>& operator*(complex<A>& complexA, complex<B>& complexB){
		typedef class promote<A,B>::res Type;
		Type real = (complexA.real*complexB.real - complexA.imag*complexB.imag);
		Type imag = (complexA.imag*complexB.real + complexA.real*complexB.imag);
		complex<Type> *next = new complex<Type>(real,imag);
		return *next;		
	}
	
//complex division
//(a+bi)/(c+di) = ((ac + bd)/(c^2+d^2)) + ((bc - ad)/(c^2+d^2))i
	template	<typename A, typename B>
	complex<typename promote<A,B>::res>& operator/(complex<A>& complexA, complex<B>& complexB){
		typedef class promote<A,B>::res Type;
		Type real = ((complexA.real*complexB.real) + (complexA.imag*complexB.imag)) / ((complexB.real*complexB.real)+(complexB.imag*complexB.imag));
		Type imag = ((complexA.imag*complexB.real) - (complexA.real*complexB.imag)) / ((complexB.real*complexB.real)+(complexB.imag*complexB.imag));
		complex<Type> *next = new complex<Type>(real,imag);
		return *next;
	}
	
//No need to promote on these as we are now only working with one number at a time	
//complex absolute value
	template	<typename A>
	A abs(complex<A>& complexA){
		A abs = sqrt((complexA.real*complexA.real) + (complexA.imag*complexA.imag));
		return abs;
	}
	
//complex conjugation
	template	<typename A>
	complex<A>& conj(complex<A>& complexA){
		A conj_real = complexA.real;
		A conj_imag = (complexA.imag * -1);
		complex<A> *conj = new complex<A>(conj_real, conj_imag);
		return *conj;
	}
	
//complex normalize
	template	<typename A>
	complex<A>& norm(complex<A>& complexA){
		A abs = sqrt((complexA.real*complexA.real) + (complexA.imag*complexA.imag));
		A norm_real = (complexA.real/abs);
		A norm_imag = (complexA.imag/abs);
		complex<A> *norm = new complex<A>(norm_real, norm_imag);
		return *norm;		
	}
	
//complex exponential
//e^(a+bi) = e^a(cos b + i sin b)
	template	<typename A>
	complex<A>& exp(complex<A>& complexA){
		A exp_real = exp(complexA.real)*(cos(complexA.imag));
		A exp_imag = exp(complexA.real)*(sin(complexA.imag));
		complex<A> *exp = new complex<A>(exp_real, exp_imag);
		return *exp;			
	}
	


int main() {
	
	complex<double> a(1.0, 2.0);//Real followed by imagniry 
	complex<float> b;
	
	cout << "Enter b: "<< endl;;
	cout << "input must be the real number followed by the imaginary "<< endl; 
	cin >> b;
	
	cout << "a = " << a << "\n";
	cout << "b = " << b << "\n";
	
	cout << "a + b = " << a + b << "\n";
	cout << "a * b = " << a * b << "\n";
	cout << "a / b = " << a / b << "\n";
	cout << "|a| = " << abs(a) << "\n";
	cout << "complex conjugate of a = " << conj(a) << "\n";
	cout << "norm of a = " << norm(a) << "\n";
	cout << "abs of a = " << abs(a) << "\n";
	cout << "exp(a) = " << exp(a) << "\n";
	
}
