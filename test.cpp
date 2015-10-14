/*
 * test.cpp
 *
 *  Created on: 27 wrz 2015
 *      Author: lukaszlampart
 */


#include "solver.h"
#include <iostream>

//simple test program

int main() {
	Sudoku grzyb(std::cin);
	Solver solverr(grzyb);
	if (solverr.solve()) {
		for (int i=0; i<9; i++) {
			for (int j=0; j<9; j++) {
				std::cout<<grzyb[i][j].field<<" ";
			}
			std::cout<<std::endl;
		}
	}
	return 0;
}
