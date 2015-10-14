/*
 * sudoku.h
 *
 *  Created on: 27 wrz 2015
 *      Author: lukaszlampart
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <bitset>
#include <array>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <istream>
#include <ostream>
#include <sstream>
#include <vector>
//PosibilitySet is alias to std::bitset<9>
typedef std::bitset<9> PosibilitySet;


/*
 * Sudoku class defines class Field and wraps array 9*9 of that class.
 * Class Field covers some functionalities of PosibilitySet,
 * It can be cast from PosibilitySet to Sudoku::Field
 * Class Sudoku has only one constructor, which get's std:istream from which it read's input,
 *
 */
class Sudoku {

public:
	enum class Number : std::int8_t { ONE = 1, TWO, THREE , FOUR , FIVE , SIX , SEVEN , EIGHT , NINE };//For values safety in sudoku table

	//Reads input, throws exception if there is something wrong with the formatting of the input, (good formating is for example:
	// 1 2 3 0 0 4 5 7 0
	// 0 3 7 9 2 0 0 0 0 etc. where numbers from 1 to 9 are values in the table, and 0 is field left free
	//Warning: there is no checking of proper data type for the input stream so if u pass it a character like 'a'
	//this constructor throws std::logic_error if data formated badly
	Sudoku(std::istream& input) {
		constexpr static std::array<Number,9> initTable={{Number::ONE,Number::TWO,Number::THREE,Number::FOUR,Number::FIVE,Number::SIX,Number::SEVEN,Number::EIGHT,Number::NINE}};
		for (int i=0; i<81; i++) {
			std::int16_t tmp;
			input>>tmp;
			if (tmp==0) {
				content[i/9][i%9]=Field(fullOption());//Set of the field is set with the proper value
			}
			else if (tmp>0 && tmp<10) {
				content[i/9][i%9]=Field(initTable[tmp-1]);
			}
			else{
				std::stringstream tmp;
				tmp<<"Wrong formating of input, line: "<<i/9<<" column: "<<i%9<<" Input: "<<tmp<<"\n";
				throw std::logic_error(tmp.str());
			}
		}
		for (int i=0; i<9; i++) {
			row[i]=column[i]=rect[i/3][i%3]=fullOption();
		}
	}
	// simple function returning  Sudoku::Number from std::size_t
	static Number getNumber(const size_t& val) {
		constexpr static std::array<Number,9> initTable={{Number::ONE,Number::TWO,Number::THREE,Number::FOUR,Number::FIVE,Number::SIX,Number::SEVEN,Number::EIGHT,Number::NINE}};
		if (val<1 || val>9) {
			std::stringstream tmp;
			tmp<<"Wrong value is tried to be set: "<<val;
			throw std::logic_error(tmp.str());
		}
		else return initTable[val-1];
	}
	// Methods below should throw some kind of "out of range" exception when passed wrong argument (only ind<=9 returns proper set)
	static PosibilitySet singleOption(size_t ind)  {
		size_t i=1;
		i<<=ind-1;
		return PosibilitySet(i);
	}
	static PosibilitySet singleOptionMask(size_t ind) {
		size_t i=1;
		i<<=ind-1;
		return PosibilitySet(1023^i);
	}
	static PosibilitySet fullOption() {
		return PosibilitySet(1023);
	}
	// Some testing methods left from "development" process
	static const char* test(const PosibilitySet& a,const PosibilitySet& b) {
		Field af(a);
		Field bf(b);
		Field cf=Number::ONE;
		try {
			cf&=af;
		} catch (std::exception& e) {
			return e.what();
		}
		return "no exception\n";
	}
	static const char* testv2(bool type) {
		Field a(Number::ONE);
		Field b(Number::TWO);
		try {
			if (type) {
				a&=b;
			}
			else {
				auto c=a&b;
			}
		} catch (std::exception& e) {
			return e.what();
		}
		return "no exception";
	}

	/*
	 *Class Field defines an common interface to both empty and filled Fields in sudoku table.
	 *It's used for implementing algorithm therefore i don't recommend using it somewhere else.
	 *Class Field is meant to allow for easy manipulation of contents of sudoku
	 *Important note: operator/= have different logic than other operators used in this class
	 *
	 */
	class Field {
	public:
		Field():filled(false),value(),set(){}
		Field(Number initValue):filled(true),value(static_cast<std::int16_t>(initValue)),set() {}
		Field(const PosibilitySet& init): filled(false),value(),set(init) {}
		Field(const PosibilitySet& a,const PosibilitySet& b): filled(false),value(),set(a) {
			set&=b;
		}
		//This operator ...
		Field& operator/=(const Field& b) {
			if (filled) throw std::logic_error("Accessing set of the filled field");
			if (b.filled) {
				set&=singleOptionMask(b.value);
			}
			return *this;
		}
		Field& operator&=(const Field& b) {
			if (filled) throw std::logic_error("Accesing set of the filled field");
			if (!b.filled) {
				set&=b.set;
			}
			return *this;
		}
		Field& operator|=(const Field& b) {
			if (filled) throw std::logic_error("Accesing set of the filled field");
			if (!b.filled) {
				set|=b.set;
			}
			return *this;
		}
		friend Field operator&(const Field& a,const Field& b) {
			if (a.filled && b.filled) throw std::logic_error("Can't interpret intersection of two undefined sets");
			if (a.filled) return Field(b);
			if (b.filled) return Field(a);
			return Field(a.set,b.set);
		}
		friend std::ostream& operator<<(std::ostream& os,const Field& obj) {
			if (!obj.filled) os<<'0';
			else os<<obj.value;
			return os;
		}
		size_t count() const{
			if (filled) return 0;
			else return set.count();
		}
		std::string toString() const {
			return set.to_string();
		}

		std::vector<std::uint16_t> values() {
			std::vector<std::uint16_t> vars;
			for (std::size_t i=0; i<9; i++) {
				if (set[i]) vars.push_back(i+1);
			}
			return vars;
		}

		bool empty() const{
			return !filled;
		}
		operator std::int16_t() {
			if (!filled) throw std::runtime_error("Geting value of empty field\n");
			return value;
		}
	private:
		bool filled;
		std::int16_t value;
		PosibilitySet set;

	};

	/*
	 * Here begins implementation of special interface of class Sudoku, it allows for easy access of the Field at position (i,j)
	 * but also provides easy acces to the "virtual" fields of it's row/column and 3x3 square,
	 */

	class ArrayInterface;
	inline ArrayInterface operator[](size_t ind) {
		return ArrayInterface(content[ind],row,column,rect[ind/3],ind);
	}
	class AccesInterface;
	/*
	 * Class ArrayInterface cannot be created/copied/(i didn't blocked move)
	 * It is made to imitate behavior of two dimensional array for our sudoku
	 */
	class ArrayInterface {
		typedef std::array<Field,9>& nineRef;
		typedef std::array<Field,3>& threeRef;
		friend class Sudoku;
	public:
		inline AccesInterface operator[](size_t ind) {
			return AccesInterface(field[ind],row[numb],column[ind],rect[ind/3]);
		}
	private:
		nineRef field;
		nineRef row;
		nineRef column;
		threeRef rect;
		size_t numb;
		ArrayInterface(nineRef iField,nineRef iRow,nineRef iColumn,threeRef iRect,size_t iNumb):field(iField),row(iRow),column(iColumn),rect(iRect),numb(iNumb) {}
		ArrayInterface();
		ArrayInterface(const ArrayInterface&);
		ArrayInterface& operator=(const ArrayInterface&);
	};

	/**
	 * Class AccesInterface provides easy access to the field of the sudoku 9x9 table, treating it
	 * as struct containing four other fields. Setting field impacts other fields in it's row/column/square
	 * For each of those categories we have virtual fields allowing for easy changing possibilities and what's
	 * more important restoring change when needed
	 */
	class AccesInterface {
	public:
		Field& field;
		Field& row;
		Field& column;
		Field& rect;
	private:
		friend class ArrayInterface;
		AccesInterface(Field& a,Field& b,Field& c,Field& d) : field(a),row(b),column(c),rect(d) {}
		AccesInterface();
	};


private:
	std::array<std::array<Field,9>,9> content;
	std::array<Field,9> row;
	std::array<Field,9> column;
	std::array<std::array<Field,3>,3> rect;
};


#endif /* SUDOKU_H_ */
