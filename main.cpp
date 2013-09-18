/* 
	Copyright © 2013 Reagan Lopez
	[This program is licensed under the "MIT License"]
*/

/*****************************************************************/
// main.cpp: Program to solve Sudoku level 0
// Takes an input file insudoku.txt with the unsolved sudoku board.
// Generates output file outsudoku.txt with the solved sudoku board.
// Prints a message if not solvable within level 0.
// Author: Reagan Lopez
// Date: 08/14/2013
/*****************************************************************/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;
#define SMALL_N 3
#define BIG_N ( SMALL_N * SMALL_N )
#define EOL '\n'
#define EMPTY 0
#define NO_CHAR ' '
#define FALSE 0
#define TRUE  1
#define DC 999
#define P_BAR		printf( "|" )
#define P_LINE		printf( "-" )
#define P_PLUS		printf( "+" )
#define P_EOL   	printf( "\n" )
#define P_TAB		printf( "\t" )
#define P_BLANK	printf( " " )
#define SUM 405
ofstream outfile;

#define ASSERT( cond , statement )												                    \
	if ( cond == NULL ) {                                                                           \
	    cout << "\n" << statement << SRC->field;                                                    \
	    outfile << "\n" << statement;                                                               \
        exit( 0 );                                                                                  \
	}

typedef struct board_tp
{
	unsigned	field;				    // 0 if empty, else one of 1..n*n
	unsigned	option_count;	 	    // initially, all n*n are options
	unsigned	can_be[ BIG_N + 1 ];	// if false, number "i" impossible
} struct_board_tp;
// this is the complete sudoku board, n*n by n*n = BIG_N * BIG_N fields
struct_board_tp sudoku[ BIG_N ][ BIG_N ];
struct_board_tp *SRC;

typedef char row_col_anal_tp;
row_col_anal_tp row_analysis = 'row';
row_col_anal_tp col_analysis = 'col';

/*****************************************************************/
// Function to validate if the solution is correct.
/*****************************************************************/
unsigned sanity_check()
{
    unsigned errors = 0;
    for ( int row = 0; row < BIG_N; row++ )
        for ( int col = 0; col < BIG_N; col++ )
            errors += sudoku[row][col].field;
    return errors;
}

/*****************************************************************/
// Function to count the number of possible candidates for a given
// position (row,col).
/*****************************************************************/
unsigned count_fields( int row, int col )
{
    unsigned cnt = 0;
    for( int i = 0; i <= BIG_N; i++ ) {
//        if ( sudoku[row][col].can_be[i] == 1 ) {
        if ( SRC->can_be[i] == TRUE ) {
            cnt++;
        }
    }
    return cnt;
}

/*****************************************************************/
// Function to find the only one possible candidate for a given
// position (row,col).
/*****************************************************************/
unsigned find_1_field( int row, int col )
{
    for( unsigned i = 0; i <= BIG_N; i++ ) {
//        if ( sudoku[row][col].can_be[i] == 1 ) {
        if ( SRC->can_be[i] == TRUE ) {
            return i;
        }
    }
    cout << "\nMultiple options for a field!";
}

/*****************************************************************/
// Function to fing the column of a subsquare.
/*****************************************************************/
int f ( int row )
{
    return ( ( row / SMALL_N ) * SMALL_N );
}

/*****************************************************************/
// Function to do the following:
// check all horizontal and vertical lines for empty spaces.
// each empty space initially has BIG_N options but for each
// existing value in that row or col, decrease the option.
// if per chance the options end up just 1,
// then we can plug in a number.
// return the number of fields changed from empty to a new value
/*****************************************************************/
unsigned horiz_vert( row_col_anal_tp row_or_col )
{ // horiz_vert
	unsigned changes = 0;
	unsigned options = 0;
	unsigned field   = 0;	// remember the next number to be excluded

	for ( int row = 0; row < BIG_N; row++ ) {
				for ( int col = 0; col < BIG_N; col++ ) {
                        SRC = &sudoku[row][col];
					if ( SRC->field ) {	// there is a number
						ASSERT( ( SRC->option_count == 0 ), "has # + option?" );
					}else{
						// field is EMPTY. Goal to count down options to 1
						ASSERT( ( SRC->option_count ), "0 field must have opt" );
                        // go thru each field. For # found, exclude # from can_be[]
                        for ( int i = 0; i < BIG_N; i++ ) {
                            if ( row_or_col == row_analysis ) {
                                field = sudoku[ row ][ i ].field;
                            }else{
                                // column analysis
                                field = sudoku[ i ][ col ].field;
                            } //end if
                            if ( field ) {
                                // we found a field
                                SRC->can_be[ field ] = FALSE;
                            } //end if
                            SRC->option_count = options = count_fields( row, col );
                            if ( options == 1 ) {
                                // plug in only 1 of BIG_N numbers
                                // and set option_count to 0
                                field = find_1_field( row, col );
                                //FILL();
                                SRC->field = field;
                                SRC->can_be[ field ] = FALSE;
                                SRC->option_count = 0;
                                changes++;
                            } //end if
                        } //end for i
                    } //end if
            } //end for col
        } //end for row
    return changes;
} //end horiz_vert

/*****************************************************************/
// Function to do the following:
// check all horizontal and vertical lines for empty spaces.
// each empty space initially has BIG_N options
// But for each field value in subsquare, decrease options.
// if per chance the options end up just 1,
// then we can plug in a number.
// return the number of fields changed from empty to a new value
/*****************************************************************/
unsigned subsquare( )
{ // subsquare
	unsigned changes = 0;
	unsigned options = 0;
	unsigned field   = 0;	// remember the next number to be excluded

	for ( int row = 0; row < BIG_N; row++ ) {
		for ( int col = 0; col < BIG_N; col++ ) {
            SRC = &sudoku[row][col];
			if ( SRC->field ) {	// there is a number
				ASSERT( ( SRC->option_count == 0 ), "has # + option?" );
			}else{
				// field is EMPTY. Goal to count down options to 1
				ASSERT( ( SRC->option_count ), "subsquare must have opt" );
				// analyze all fields in subsquare, exclude from can_be[]
				for ( int i = f( row ); i < ( f( row ) + SMALL_N ); i++ ) {
					ASSERT( ( i <= row+SMALL_N ), "wrong i_row in [][]" );
					for ( int j = f( col ); j < ( f( col ) + SMALL_N ); j++ ) {
						ASSERT( j <= col+SMALL_N, "wrong j_col in [][]" );
						field = sudoku[ i ][ j ].field;
						if ( field ) {
							// we found a non-zero field
							SRC->can_be[ field ] = FALSE;
						} //end if
						SRC->option_count = options = count_fields( row, col );
						if ( options == 1 ) {
							// plug in only 1 of BIG_N numbers
							// and set option_count to 0
							field = find_1_field( row, col );
							//FILL();

                            SRC->field = field;
                            SRC->can_be[ field ] = FALSE;
                            SRC->option_count = 0;
                            changes++;
						} //end if
					} //end for j
				} //end for i
           } //end if
	  } //end for col
	} //end for row
	return changes;
} //end subsquare

/*****************************************************************/
// Function to do the following:
// simplest sudoku strategy by eliminating options for a field
// that would conflict with existing numbers
// in row, column, subsquare.
/*****************************************************************/
unsigned sudoku_level0()
{ //sudoku_level0
	unsigned changes;			// count fields filled in
	unsigned iterations = 0;	// count times we go around
	unsigned errors = 0;		// do final sanity check

	do {
				changes  = 0;
				changes  = horiz_vert( row_analysis );
				changes += horiz_vert( col_analysis );
				changes += subsquare();
				++iterations;
	} while ( changes );
//	try_single_option();

//    printf( "\nIterated level-0 %d times.", iterations );
    errors = sanity_check();

    if ( errors != SUM ) {
        cout << "\nField was not solvable with level 0.";
        outfile << "\nField was not solvable with level 0.";
        outfile.close();
        exit( 0 );
    }

    cout << endl;
	return changes;
} //end sudoku_level0


/*****************************************************************/
// Main function.
// Reads input file with unsolved board.
// Calls the function to solve the sudoku level 0.
// Prints the output file with the input and output sudoku board.
/*****************************************************************/
int main()
{

    int filled = 0, emp = 0;
    string input;
    int j, prev, cur, cnt = 0;
    ifstream infile( "insudoku.txt" ); // open input file
    outfile.open( "outsudoku.txt" ); // open output file

	// read input file character by character.
	// print to output file as read.
    if ( infile.is_open()) {
        cout << "\nINPUT:";
        outfile << "\nINPUT:";
        cout << "\n  0 1 2  3 4 5  6 7 8   ";
        outfile << "\n  0 1 2  3 4 5  6 7 8   ";
        cout << "\n+------+------+------+  ";
        outfile << "\n+------+------+------+  ";
        while ( !infile.eof() ) { // read input till end of file.
           j = 0; prev = 0; cur = 0;
           getline (infile,input);
           const char *p = input.c_str();
           cout << "\n|";
           outfile << "\n|";
           while(*p != '\0') { // read input till end of line.
               if (*p == ' ') {
                   sudoku[cnt][j].field = atoi(input.substr(prev,cur-prev).c_str());
                   if ( sudoku[cnt][j].field == 0 ) {
                        ++emp;
                       sudoku[cnt][j].option_count = BIG_N;
                       for ( int s = 0; s <= BIG_N; s++ ) {
                            if ( s == 0 ) {
                               sudoku[cnt][j].can_be[s] = DC;
                            }else{
                                sudoku[cnt][j].can_be[s] = TRUE;
                            }
                       } //end for
                   }else{
                       ++filled;
                       sudoku[cnt][j].option_count = 0;
                       for ( int s = 0; s <= BIG_N; s++ ) {
                            if ( s == 0 ) {
                               sudoku[cnt][j].can_be[s] = DC;
                            }else if ( s == sudoku[cnt][j].field ) {
                                sudoku[cnt][j].can_be[s] = TRUE;
                            }else{
                               sudoku[cnt][j].can_be[s] = FALSE;
                            }
                       } //end for
                   } //end if
                   char c = ' ';
                    cout << " " << sudoku[cnt][j].field;
                    outfile << " " << sudoku[cnt][j].field;
                    if ( ( j + 1 ) % 3 == 0 ) {
                        cout << "|";
                        outfile << "|";
                    } //end if

                   prev = cur + 1;
                   j++;
               } //end if
               cur++;
               *p++;
           } //end while

           sudoku[cnt][j].field = atoi(input.substr(prev,cur-prev).c_str());
           if ( sudoku[cnt][j].field == 0 ) {
                ++emp;
               sudoku[cnt][j].option_count = BIG_N;
               for ( int s = 0; s <= BIG_N; s++ ) {
                    if ( s == 0 ) {
                       sudoku[cnt][j].can_be[s] = DC;
                    }else{
                        sudoku[cnt][j].can_be[s] = TRUE;
                    }
               }
           }else{
               ++filled;
               sudoku[cnt][j].option_count = 0;
               for ( int s = 0; s <= BIG_N; s++ ) {
                    if ( s == 0 ) {
                       sudoku[cnt][j].can_be[s] = DC;
                    }else if ( s == sudoku[cnt][j].field ) {
                        sudoku[cnt][j].can_be[s] = TRUE;
                    }else{
                       sudoku[cnt][j].can_be[s] = FALSE;
                    }
               }
           } //end if
                cout << " " << sudoku[cnt][j].field;
                outfile << " " << sudoku[cnt][j].field;
                if ( ( j + 1 ) % 3 == 0 ) {
                    cout << "| " << cnt;
                    outfile << "| " << cnt;
                } //end if
                if ( ( ( cnt + 1 ) % 3 ) == 0 )  {
                    cout << "\n+------+------+------+  ";
                    outfile << "\n+------+------+------+  ";
                } //end if
           cnt++;
        } //end while

        cout << "\nStatistics initially: ";
        cout << "\nTotal # of fields: " << filled + emp;
        cout << "\nFields filled: " << filled;
        cout << "\nEmpty fields: " << emp;
        outfile << "\nStatistics initially: ";
        outfile << "\nTotal # of fields: " << filled + emp;
        outfile << "\nFields filled: " << filled;
        outfile << "\nEmpty fields: " << emp;

    }else{
        cout << "\nUnable to open input file!";
    } //end if
    infile.close(); // close input file

    sudoku_level0(); // call sudoku level 0 funtion


	// print output file.
    filled = 0;
    emp = 0;
    cout << "\n\nOUTPUT:";
    outfile << "\n\nOUTPUT:";
    cout << "\n  0 1 2  3 4 5  6 7 8   ";
    outfile << "\n  0 1 2  3 4 5  6 7 8   ";
    cout << "\n+------+------+------+  ";
    outfile << "\n+------+------+------+  ";
	for ( int row = 0; row < BIG_N; row++ ) {
        cout << "\n|";
        outfile << "\n|";
        for ( int col = 0; col < BIG_N; col++ ) {
            if ( sudoku[row][col].field == 0 ) {
                ++emp;
            }else{
                ++filled;
            }
            cout << " " << sudoku[row][col].field;
            outfile << " " << sudoku[row][col].field;
            if ( ( col + 1 ) % 3 == 0 ) {
                cout << "|";
                outfile << "|";
            }
        }
        cout << " " << row;
        outfile << " " << row;
        if ( ( row + 1 ) % 3 == 0 ) {
            cout << "\n+------+------+------+  ";
            outfile << "\n+------+------+------+  ";
        }
	}//end for
    cout << "\nStatistics Sudoku level 0: ";
    cout << "\nTotal # of fields: " << filled + emp;
    cout << "\nFields filled: " << filled;
    cout << "\nEmpty fields: " << emp;
    outfile << "\nStatistics Sudoku level 0: ";
    outfile << "\nTotal # of fields: " << filled + emp;
    outfile << "\nFields filled: " << filled;
    outfile << "\nEmpty fields: " << emp;
	cout << "\nField was solvable with level 0.";
	outfile << "\nField was solvable with level 0.";


    outfile.close(); // close output file.

    return 0;
}
