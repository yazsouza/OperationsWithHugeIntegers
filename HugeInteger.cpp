#include "HugeInteger.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <random>

HugeInteger::HugeInteger(const std::string& val) {

	if (val == "")
		throw std::invalid_argument("empty string");

	if (val[0] == '-') { //if the first value of the string is a -
		neg = 1; // make the value of bool neg = true
		size++;

		for (int i = 1; i < val.length(); i++) { // iterate through string val starting at second element
			if (0 <= (val[i] - 48) && (val[i] - 48) <= 9) { // if valid number (subtract 48 to translate from ASCII)
				vect.push_back(val[i] - 48);//add element i of val to vector 
				size++;
			}
			else
				throw std::invalid_argument("invalid argument"); //else throw exception
		}
	}

	else {
		neg = 0; //make neg = false
		for (int i = 0; i < val.length(); i++) { // if not negative, do same thing as above but start from first element
			if (0 <= (val[i] - 48) && (val[i] - 48) <= 9) { //while the element of val is a digit
				vect.push_back(val[i] - 48);// add element i of val to vector (subtract 48 to translate from ASCII)
				size++;
			}
			else
				throw std::invalid_argument("invalid argument");
		}
	}
}


HugeInteger::HugeInteger(int n) {
	if (n <= 0) //if n is less than or equal to 0 throw exception
		throw std::invalid_argument("invalid argument");

	int v1 = (rand() % 9) + 1; //get random digit from 1-9 for firrst element of vector
	vect.push_back(v1);

	for (int i = 1; i < n; i++) { //get random digit from 0-9 for remaining elements
		int v = rand() % 10;
		vect.push_back(v);
	}
}

HugeInteger HugeInteger::add(const HugeInteger& h, int pos) const {

	if (pos == 0) {
		if (neg == 0 && h.neg == 1) // If this hugeint is positive and h is negative, subtract
			return subtract(h, 1);

		if (neg == 1 && h.neg == 0)
			return h.subtract(*this, 1);	// If this hugeint is negative and h is positive, subtract this from h 
	}

	int size = (h.vect.size() >= vect.size()) ? h.vect.size() : vect.size(); //makes size the bigger size
	int carry = 0, offset, index;
	HugeInteger sum = HugeInteger("0"); //create new hugeint for sum

	for (int i = 0; i < size - 1; i++)
		sum.vect.push_back(0); //create vector of 0s

	// If both numbers are negative, the result will also be negative.
	if (neg && h.neg)
		sum.neg = true;


	for (offset = 1; offset <= size; offset++) {
		index = size - offset; //starting at LSD

		if (vect.size() >= offset)
			sum.vect[index] += vect[vect.size() - offset];

		if (h.vect.size() >= offset)
			sum.vect[index] += h.vect[h.vect.size() - offset];

		if (carry)
			sum.vect[index] += carry--; //add carry if applicable

		if (sum.vect[index] > 9) { //of sum greater than 10
			sum.vect[index] %= 10;
			carry++; //increment carry for next iteration
		}
	}


	if (carry) sum.vect.insert(sum.vect.begin(), carry--);	// If there is still a carry, add it as most SD

	return sum;
}


HugeInteger HugeInteger::subtract(const HugeInteger& h, int pos) const {

	if (pos == 0) {

		if (!neg && h.neg) return add(h, 1);// If this hugeint pos and h is neg, add 

		if (neg && !h.neg) return add(h, 1).invert();// If this hugeint neg and h is pos, add and result make neg

		if (neg && h.neg) {// If this num and h are neg, subtract frorm h instead.
			HugeInteger diff = h.subtract(*this, 1);
			if (h.compareMagnitude(*this) == 1)// If h is bigger than this hugeint, negate the result.
				diff.invert();
			return diff;
		}
	}

	const HugeInteger& bigger = compareMagnitude(h) == -1 ? h : *this; //create refernce to bigger num
	const HugeInteger& smaller = compareMagnitude(h) == -1 ? *this : h; //create reference to smaller num

	int size = bigger.vect.size();

	int borrow = 0, offset, i, smallIndex;

	HugeInteger diff = HugeInteger("0"); // create a new hugeint to store the result
	for (int i = 0; i < size - 1; i++) diff.vect.push_back(0);

	if (bigger.neg)
		diff.neg = true; //if bigger num negative, result neg

	if (compareTo(h) == -1)
		diff.neg = true; //if h is larger, result neg


	for (offset = 1; offset <= size; offset++) { //starting at LSD
		i = size - offset; //decrements index each iteration
		smallIndex = smaller.vect.size() - offset;

		if (smallIndex >= 0) {
			if (bigger.vect[i] >= smaller.vect[smallIndex] + borrow) { //if bigger num in bigger vect is larger than smaller + borrow 
				diff.vect[i] = bigger.vect[i] - smaller.vect[smallIndex] - borrow; //subtract smaller from bigger
				borrow = 0;
			}
			else {
				diff.vect[i] = bigger.vect[i] + 10 - smaller.vect[smallIndex] - borrow; //use 10 as borrow and set borrow
				borrow = 1;
			}
		}
		else {
			if (bigger.vect[i] >= borrow) {
				diff.vect[i] = bigger.vect[i] - borrow; //subtract borrow from bigger num
				borrow = 0;
			}
			else {
				diff.vect[i] = bigger.vect[i] + 10 - borrow;
				borrow = 1;
			}
		}
	}


	while (diff.vect.size() > 1 && diff.vect[0] == 0) {	// remove leading zeros.
		diff.vect.erase(diff.vect.begin());
	}


	if (diff.vect[0] == 0) diff.neg = false; //if result is 0, make positive.

	return diff;
}



HugeInteger HugeInteger::multiply(const HugeInteger& h) const {
	int firstLength = vect.size();
	int secondLength = h.vect.size();

	int index1 = 0; //initalize indecies
	int index2 = 0;
	std::vector<int> total(firstLength + secondLength); //for final result

	for (int i = firstLength - 1; i >= 0; i--) //begin at LSD
	{
		int carry = 0; //carry for future iterations

		index2 = 0;
		for (int j = secondLength - 1; j >= 0; j--) //to multiply every digit of second num w every digit of first num
		{
			int digit = vect[i] * h.vect[j] + total[index1 + index2] + carry;
			carry = digit / 10; //first digit is carry
			total[index1 + index2] = digit % 10; //give resultant
			index2++;
		}
		if (carry > 0) //if there is still a carry
			total[index1 + index2] += carry; //add carry to next num

		index1++; //incrememnt index of the first number
	}
	int i = total.size() - 1;

	while (i >= 0 && total[i] == 0) {
		i--; //ignore extra 0s
	}
	if (i == -1) {
		return HugeInteger("0"); //if i reaches -1, num was 0
	}
	std::string multiply = ""; //create the empty string
	if (neg == true && h.neg == false || neg == false && h.neg == true) //if one of the numbers is negative, add a negative to the final
		multiply += "-";
	while (i >= 0)
		multiply += std::to_string(total[i--]);  // save to string

	return HugeInteger(multiply);
}


int HugeInteger::compareTo(const HugeInteger& h) const {
	int len1 = h.vect.size();
	int len2 = this->vect.size();
	int length = 0;
	int i = 0;

	//one neg one pos

	if (this->neg == 1 && h.neg == 0)
		return -1;
	if (this->neg == 0 && h.neg == 1)
		return 1;

	if (len1 >= len2)
		length = len1;
	if (len2 < len1)
		length = len2;

	//both positive

	if (this->neg == 0 && h.neg == 0) {
		if (len1 > len2)
			return -1;

		else if (len2 > len1)
			return 1;

		else for (i; i < len1; i++) {
			if (vect[i] > h.vect[i])
				return 1;
			if (h.vect[i] > vect[i])
				return -1;
		}
		if (i == len1)
			return 0;
	}

	//both negative

	if (this->neg == 1 && h.neg == 1) {
		if (len1 < len2)
			return -1;

		else if (len2 < len1)
			return 1;

		else for (i; i < len1; i++) {
			if (vect[i] < h.vect[i])
				return 1;
			if (h.vect[i] < vect[i])
				return -1;
		}
		if (i == len1)
			return 0;
	}
}

int HugeInteger::compareMagnitude(const HugeInteger& h) const {
	// Check for size
	if (vect.size() > h.vect.size()) return 1;
	if (vect.size() < h.vect.size()) return -1;

	// Check for each digit
	for (int i = 0; i < vect.size(); i++) {
		if (vect[i] > h.vect[i]) return 1;
		if (vect[i] < h.vect[i]) return -1;
	}

	return 0;
}


std::string HugeInteger::toString() const {
	std::string numstring = ""; // initialize empty string 

	for (int i = 0; i < vect.size(); i++) {  // iterate through vector and add each element to string
		numstring += std::to_string(vect[i]);
	}

	if (neg == 1) //if negative, add '-' to beginning of string
		numstring = '-' + numstring;

	return numstring;
}

HugeInteger HugeInteger::invert() {
	neg = !neg;
	return *this;
}