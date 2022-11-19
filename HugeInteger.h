#ifndef HUGEINTEGER_H_
#define HUGEINTEGER_H_
#include <iostream>
#include <vector>

class HugeInteger
{
private:
	bool neg;
	std::vector<int> vect;
	int size = 0;

public:
	HugeInteger(const std::string& val);
	HugeInteger(int n);
	HugeInteger add(const HugeInteger& h, int pos = 0)const;
	HugeInteger subtract(const HugeInteger& h, int pos = 0)const;
	HugeInteger multiply(const HugeInteger& h)const;
	int compareTo(const HugeInteger& h) const;
	std::string toString() const;
	int compareMagnitude(const HugeInteger& h) const;
	HugeInteger invert();
;
};

#endif /* HUGEINTEGER_H_ */