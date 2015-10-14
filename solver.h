/*
 * solver.h
 *
 *  Created on: 28 wrz 2015
 *      Author: lukaszlampart
 */

#ifndef SOLVER_H_
#define SOLVER_H_
#include "sudoku.h"
#include <iostream>

class Solver {
public:
	Solver(Sudoku& input): sudoku(input) {}
	//method returning pair<bool,Sudoku> telling whether given riddle can be solved, and containing solved table
	bool solve() {
		for (int i=0; i<9; i++) {
			for (int j=0; j<9; j++) {
				sudoku[i][j].column/=sudoku[i][j].field;
				sudoku[i][j].row/=sudoku[i][j].field;
				sudoku[i][j].rect/=sudoku[i][j].field;
			}
		}

		// returning pair bool,Sudoku
		try {
		return recursiveSolver();
		} catch (std::exception& e) {
			std::stringstream tmp(e.what());
			tmp<<std::endl;
			tmp<<"Exiting solve function, fatal error"<<std::endl;
			throw std::exception(tmp.str());
		}
	}
private:
	Sudoku& sudoku;
	typedef std::pair<std::uint16_t,std::uint16_t> indPair;
	indPair getMin() {
		std::uint16_t im=10,jm=10;
		size_t mx=10;
		for (std::uint16_t i=0; i<9; i++) {
			for (std::uint16_t j=0; j<9; ++j) {
				if (sudoku[i][j].field.empty()) {
					auto g=sudoku[i][j];
					g.field=g.column;
					g.field&=g.row;
					g.field&=g.rect;
					auto val=g.field.count();
					if (val<mx) {
						mx=val;
						im=i;
						jm=j;
					}
				}
			}
		}
		return indPair(im,jm);
	}


	//recursive method for solving sudoku
	bool recursiveSolver() {
		auto ind=getMin();// get index of the field of the minimal options
		if (ind==indPair{10,10}) return true; // no free field left = sudoku solved
		auto set=sudoku[ind.first][ind.second].field;
		//
		//
		if (set.count()==0) return false;
		auto values=set.values();
		for (auto i: values) {
			//
			//
			typename Sudoku::Field var(Sudoku::singleOption(i));
			//deleting options from the sets
			//optimization of access to the proper fields
			// operator /= has wrong implementation
			auto smartRef=sudoku[ind.first][ind.second];
			smartRef.rect/=Sudoku::getNumber(i);
			smartRef.column/=Sudoku::getNumber(i);
			smartRef.row/=Sudoku::getNumber(i);
			try {
				sudoku[ind.first][ind.second].field=Sudoku::getNumber(i);
				auto flag=recursiveSolver();
				if (flag) return true; // sudoku was solved pass the information
			} catch (std::logic_error& error) {
				std::stringstream tmp;
				tmp<<error.what();
				tmp<<" at position: "<<ind.first<<" "<<ind.second;
				throw std::logic_error(tmp.str());
			}
			//restoring the state from before entering the recursion
			smartRef.field=Sudoku::Field();
			smartRef.row|=var;
			smartRef.rect|=var;
			smartRef.column|=var;
		}
		// if we have not returned from the recursion with true value return false
		return false;
	}
};


#endif /* SOLVER_H_ */
