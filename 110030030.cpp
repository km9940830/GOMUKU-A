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


class node {
    public:
    node(){}
    node(double v,int t):value(v),turn(t){
        moved.clear();
        moveList.clear();
    };
    double value;
    int turn;
    std::vector<point> moved;
    std::vector<point> moveList;
    point bestmove;
    void generateMoves();
};

int player;
const int SIZE = 15;
int flag = 0;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> curBoard;
node alphabeta(node& cur,int depth,double alpha,double beta,bool maximizingPlayer);
double getScore(bool blacksTurn);
int evaluateALL(bool forblack,bool blacksTurn);
int evaluateHorizontal(bool forBlack, bool playersTurn );
int evaluateVertical(bool forBlack, bool playersTurn );
int evaluateDiagonal(bool forBlack, bool playersTurn );
int getConsecutiveSetScore(int count, int blocks, bool currentTurn);



void node::generateMoves(){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(curBoard[i][j]!=0) flag = 1;
            if(curBoard[i][j]>0)continue;

            if(i > 0) {
				    if(j > 0) {
						if(curBoard[i-1][j-1] > 0 ||curBoard[i][j-1] > 0) {
							point move(i,j,turn);
							moveList.push_back(move);
							continue;
						}
					}
					if(j < SIZE-1) {
						if(curBoard[i-1][j+1] > 0 ||curBoard[i][j+1] > 0) {
							point move(i,j,turn);
							moveList.push_back(move);
							continue;
						}
					}
					if(curBoard[i-1][j] > 0) {
						point move(i,j,turn);
						moveList.push_back(move);
						continue;
					}
			}
			if( i < SIZE-1) {
					if(j > 0) {
						if(curBoard[i+1][j-1] > 0 ||curBoard[i][j-1] > 0) {
							point move(i,j,turn);
						    moveList.push_back(move);
							continue;
						}
					}
					if(j < SIZE-1) {
						if(curBoard[i+1][j+1] > 0 ||curBoard[i][j+1] > 0) {
							point move(i,j,turn);
						    moveList.push_back(move);
							continue;
						}
					}
					if(curBoard[i+1][j] > 0) {
						point move(i,j,turn);
						moveList.push_back(move);
						continue;
					}
			}
        }
    }
    if(flag==0){
        std::cout << "ZERO "<< std::endl;
        point move(7,7,turn);
        moveList.push_back(move);
    }
}


node alphabeta(node& cur,int depth,double alpha,double beta,bool maximizingPlayer){
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            curBoard[i][j] = board[i][j]; 
        }
    }
    for(auto it : cur.moved){
        curBoard[it.x][it.y] = it.Turn;
    }
    if(depth == 0){
        //cur.value = rand() % 100;
        cur.value = getScore(!maximizingPlayer);
        std::cout << cur.value << std::endl;
        return cur;
    }
    
    cur.generateMoves();
    if(cur.moveList.empty()){
        //cur.value = rand() % 100;
        cur.value = getScore(!maximizingPlayer);
        std::cout << cur.value << std::endl;
        return cur;
    }
    if(maximizingPlayer){
        cur.value = std::numeric_limits<int>::min();
        for(auto& it:cur.moveList){
            node next(std::numeric_limits<int>::max(),2);
            next.moved = cur.moved;
            next.moved.push_back(it);
            double prevalue = cur.value;
            cur.value = std::max(cur.value,alphabeta(next,depth-1,alpha,beta,!maximizingPlayer).value);
            if(cur.value > prevalue){
                cur.bestmove = it;
            }
            alpha = std::max(alpha,cur.value);
            if(alpha >= beta)
                break;
        }
        return cur;
    }
    else{
        cur.value = std::numeric_limits<int>::max();
        for(auto& it : cur.moveList){
            node next(std::numeric_limits<int>::min(),1);
            next.moved = cur.moved;
            next.moved.push_back(it);
            double prevalue = cur.value;
            cur.value = std::min(cur.value,alphabeta(next,depth-1,alpha,beta,!maximizingPlayer).value);
            if(cur.value < prevalue){
                cur.bestmove = it;
            }
            beta = std::min(beta,cur.value);
            if(beta <= alpha){
                break;
            }
        }
        return cur;
    }
}

int evaluateALL(bool forblack,bool blacksTurn){
    return evaluateHorizontal(forblack, blacksTurn) +
			evaluateVertical(forblack, blacksTurn) +
			evaluateDiagonal(forblack, blacksTurn);
}

double getScore(bool blacksTurn) {
		
		double blackScore = evaluateALL(true, blacksTurn);
		double whiteScore = evaluateALL(false, blacksTurn);
		
        if(player == BLACK){
            if(whiteScore == 0) whiteScore = 1.0;
		    return blackScore / whiteScore;
        }
        else{
            if(blackScore == 0) blackScore = 1.0;
		    return whiteScore / blackScore;
        }
		
}

int evaluateHorizontal(bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		
		for(int i=0; i<SIZE; i++) {
			for(int j=0; j < SIZE; j++) {
				if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
					if(consecutive > 0) {
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
			}
			if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
			}
			consecutive = 0;
			blocks = 2;
		}

		return score;
	}
	
int evaluateVertical(bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		
		for(int j=0; j<SIZE; j++) {
			for(int i=0; i<SIZE; i++) {
				if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
					if(consecutive > 0) {
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
			}
			if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
				
			}
			consecutive = 0;
			blocks = 2;
			
		}
		return score;
}

int evaluateDiagonal(bool forBlack, bool playersTurn ) {
		
		int consecutive = 0;
		int blocks = 2;
		int score = 0;
		// From bottom-left to top-right 
		for (int k = 0; k <= 2 * SIZE; k++) {
            int istart = std::min(k,SIZE-1) ;
            int iend = std::max(0,k-SIZE+1);
		    for (int i = istart; i <= iend; i--) {
		        int j = k - i;
		        
		        if(curBoard[i][j] == (forBlack ? 1 : 2)) {
					consecutive++;
				}
				else if(curBoard[i][j] == 0) {
					if(consecutive > 0) {
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
		        
		    }
		    if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
				
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
						blocks--;
						score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
						consecutive = 0;
						blocks = 1;
					}
					else {
						blocks = 1;
					}
				}
				else if(consecutive > 0) {
					score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
					consecutive = 0;
					blocks = 2;
				}
				else {
					blocks = 2;
				}
		        
		    }
		    if(consecutive > 0) {
				score += getConsecutiveSetScore(consecutive, blocks, forBlack == playersTurn);
				
			}
			consecutive = 0;
			blocks = 2;
		}
		return score;
}

int getConsecutiveSetScore(int count, int blocks, bool currentTurn) {
		int winGuarantee = 1000000;
		if(blocks == 2 && count < 5) return 0;

		switch(count) {
		case 5: {
			return winGuarantee;
		}
		case 4: {
			if(currentTurn) return winGuarantee;
			else {
				if(blocks == 0) return winGuarantee/4;
				else return 200;
			}
		}
		case 3: {
			if(blocks == 0) {
				if(currentTurn) return 50000;
				else return 200;
			}
			else {
				if(currentTurn) return 10;
				else return 5;
			}
		}
		case 2: {
			if(blocks == 0) {
				if(currentTurn) return 7;
				else return 5;
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
        node root(std::numeric_limits<int>::min(),1);
        alphabeta(root,1,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1);
        if (board[root.bestmove.x][root.bestmove.y] == EMPTY) {
            fout << root.bestmove.x << " " << root.bestmove.y << std::endl;
            std::cout << root.bestmove.x << " " << root.bestmove.y << std::endl;
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
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
