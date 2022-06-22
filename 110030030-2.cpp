#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <vector>
#include <limits>

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

class point {
    public:
    point(){}
    ~point(){}
    point(int x,int y,int p) :x(x),y(y),Turn(p){};
    int x;
    int y;
    int Turn;
};


/*class node {
    public:
    node(){}
    node(double v,int t):value(v),turn(t){
        moved.clear();
        moveList.clear();
    };
    double value;
    int turn;
    
    point bestmove;
    
};*/

point bestmove;
int player;
const int SIZE = 15;
int flag = 0;
std::array<std::array<int, SIZE>, SIZE> board;
//std::array<std::array<int, SIZE>, SIZE> curBoard;


double alphabeta(std::array<std::array<int, SIZE>, SIZE> curboard,int depth,double alpha,double beta,bool maximizingPlayer);
double getScore(std::array<std::array<int, SIZE>, SIZE> curBoard,bool blacksTurn);
int evaluateALL(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forblack,bool blacksTurn);
int evaluateHorizontal(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn );
int evaluateVertical(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn );
int evaluateDiagonal(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn );
int getConsecutiveSetScore(int count, int blocks, bool chance);
std::vector<point> generateMoves(std::array<std::array<int, SIZE>, SIZE> Board,std::vector<point> moveList,int turn);



std::vector<point> generateMoves(std::array<std::array<int, SIZE>, SIZE> Board,int turn){
    std::vector<point> moveList;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(Board[i][j]!=0) flag = 1;
            if(Board[i][j]>0)continue;

            if(i > 0) {
				    if(j > 0) {
						if(Board[i-1][j-1] > 0 ||Board[i][j-1] > 0) {
							point move(i,j,turn);
							moveList.push_back(move);
							continue;
						}
					}
					if(j < SIZE-1) {
						if(Board[i-1][j+1] > 0 ||Board[i][j+1] > 0) {
							point move(i,j,turn);
							moveList.push_back(move);
							continue;
						}
					}
					if(Board[i-1][j] > 0) {
						point move(i,j,turn);
						moveList.push_back(move);
						continue;
					}
			}
			if( i < SIZE-1) {
					if(j > 0) {
						if(Board[i+1][j-1] > 0 ||Board[i][j-1] > 0) {
							point move(i,j,turn);
						    moveList.push_back(move);
							continue;
						}
					}
					if(j < SIZE-1) {
						if(Board[i+1][j+1] > 0 ||Board[i][j+1] > 0) {
							point move(i,j,turn);
						    moveList.push_back(move);
							continue;
						}
					}
					if(Board[i+1][j] > 0) {
						point move(i,j,turn);
						moveList.push_back(move);
						continue;
					}
			}
        }
    }
    if(flag==0){
        //std::cout << "ZERO "<< std::endl;
        point move(7,7,turn);
        moveList.push_back(move);
    }
    return moveList;
}


double alphabeta(std::array<std::array<int, SIZE>, SIZE> curboard,int depth,double alpha,double beta,bool maximizingPlayer){
    if(depth == 0){
        //std::cout << cur.value << std::endl;
        //return  rand() % 100;
        return getScore(curboard,player == BLACK ? 1:0);
        
    }
    std::vector<point> moveList = generateMoves(curboard,player==WHITE?2:1);

    if(moveList.empty()){
        //return  rand() % 100;
        return getScore(curboard,player == BLACK ? 1:0);
    }
    if(maximizingPlayer){
        double value = std::numeric_limits<int>::min();
        for(auto& it:moveList){
            std::array<std::array<int, SIZE>, SIZE> nextBoard;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    nextBoard[i][j] = curboard[i][j]; 
                }
            }
            nextBoard[it.x][it.y] = it.Turn;
            double prevalue = value;
            value = std::max(value,alphabeta(nextBoard,depth-1,alpha,beta,!maximizingPlayer));
            if(value > prevalue){
                bestmove = it;
            }
            alpha = std::max(alpha,value);
            if(alpha >= beta)
                break;
        }
        return value;
    }
    else{
        double value = std::numeric_limits<int>::max();
        for(auto& it : moveList){
            std::array<std::array<int, SIZE>, SIZE> nextBoard;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    nextBoard[i][j] = curboard[i][j]; 
                }
            }
            nextBoard[it.x][it.y] = it.Turn;
            double prevalue = value;
            value = std::min(value,alphabeta(nextBoard,depth-1,alpha,beta,!maximizingPlayer));
            if(value < prevalue){
                bestmove = it;
            }
            beta = std::min(beta,value);
            if(beta <= alpha){
                break;
            }
        }
        return value;
    }
}

int evaluateALL(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forblack,bool blacksTurn){
    return evaluateHorizontal(curBoard,forblack, blacksTurn) +
			evaluateVertical(curBoard,forblack, blacksTurn) +
			evaluateDiagonal(curBoard,forblack, blacksTurn);
}

double getScore(std::array<std::array<int, SIZE>, SIZE> curBoard,bool blacksTurn) {
		
		double blackScore = evaluateALL(curBoard,true, blacksTurn);
		double whiteScore = evaluateALL(curBoard,false, blacksTurn);
		
        if(player == BLACK){
            if(whiteScore == 0) whiteScore = 1.0;
		    return blackScore / whiteScore;
            //return blackScore - whiteScore;
        }
        else{
            //if(blackScore == 0) blackScore = 1.0;
		    //return whiteScore / blackScore;
            return whiteScore - blackScore;
        }
		
}

int evaluateHorizontal(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		
		for(int i=0; i<SIZE; i++) {
			for(int j=0; j < SIZE; j++) {
				if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
                    bool chance = 0;
					if(consecutive > 0) {
                        if(j!=(SIZE-1) && curBoard[i][j+1]==(forBlack ? 1 : 2)){
                            chance = true;
                        }
						blocks--;
						score += getConsecutiveSetScore(consecutive,chance);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, 0);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
			}
			if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, 0);
			}
			consecutive = 0;
			blocks = 2;
		}

		return score;
	}
	
int evaluateVertical(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		
		for(int j=0; j<SIZE; j++) {
			for(int i=0; i<SIZE; i++) {
				if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
                    bool chance = 0;
					if(consecutive > 0) {
                        if(i!=(SIZE-1) && curBoard[i+1][j]==(forBlack ? 1 : 2)){
                            chance = true;
                        }
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, chance);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, 0);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
			}
			if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, 0);
				
			}
			consecutive = 0;
			blocks = 2;
			
		}
		return score;
}

int evaluateDiagonal(std::array<std::array<int, SIZE>, SIZE> curBoard,bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		// From bottom-left to top-right 
		for (int k = 0; k <= 2 * (SIZE-1); k++) {
            int iend = std::min(k,SIZE-1) ;
            int istart = std::max(0,k-SIZE+1);
		    for (int i = istart; i <= iend; ++i) {
		        int j = k - i;
		        
		        if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
					if(consecutive > 0) {
                        bool chance = 0;
                        if(i!=(SIZE-1) && j!=0 && curBoard[i+1][j-1]==(forBlack ? 1 : 2)){
                            chance = true;
                        }
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, chance);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, 0);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
		        
		    }
		    if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, 0);
				
			}
			consecutive = 0;
			blocks = 2;
		}

		// From top-left to bottom-right diagonally

		for (int k = SIZE-1 ; k <= 1-SIZE; k--) {
            int istart = std::max(0,k);
            int iend = std::min(SIZE+k-1,SIZE-1);
		    for (int i = istart; i <= iend; i++) {
		        int j = i - k;
		        
		        if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
					if(consecutive > 0) {
                        bool chance = 0;
                        if(i!=(SIZE-1) && j!=(SIZE-1) && curBoard[i+1][j+1]==(forBlack ? 1 : 2)){
                            chance = true;
                        }
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, chance);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, 0);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
		        
		    }
		    if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks,0);
				
			}
			consecutive = 0;
			blocks = 2;
		}
		return score;
}

int getConsecutiveSetScore(int count, int blocks,bool chance) {
		int winGuarantee = 1000000;
		if(blocks == 2 && count < 5) return 0;

		switch(count) {
		case 5: {
			return winGuarantee;
		}
		case 4: {
			 return winGuarantee;
		}
		case 3: {
            if(blocks == 0) {
				return 50000;
                //else return 200;
			}
            else if(chance){
                return 4000;
                //else return 50;
            }
			else {
				return 10;
				//else return 5;
			}
		}
		case 2: {
			if(blocks == 0) {
				return 7;
                //else return 5;
			}
            else if(chance){
                return 5;
            }
			else {
				return 3;
			}
		}
		case 1: {
			return 1;
		}
		}
		return winGuarantee*2;
	}




//////////////////////////////////////////////////////////////

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    //int x, y;
    // Keep updating the output until getting killed.
    while(true) {
        // Choose a random spot.
        //int x = (rand() % SIZE);
        //int y = (rand() % SIZE);
        alphabeta(board,1,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1);
        if (board[bestmove.x][bestmove.y] == EMPTY) {
            fout << bestmove.x << " " << bestmove.y << std::endl;
            //std::cout << bestmove.x << " " << bestmove.y << std::endl;
            // Remember to flush the output to ensure the last action is written to file.
            fout.flush();
            break;
        }
    }
}





int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    std::cout << player << std::endl;
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
