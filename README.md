# Sudoku-Solver
My simple library for solving Sudoku. test.cpp is an example of using this small library. The only way for importing data is passing std::istream to the constructor. This way to use the solver properly u just need to format input data as follows:
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
# # # # # # # # #
where # is number 0 (empty field) or one of {1,2,...,9}
