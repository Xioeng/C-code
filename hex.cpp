#include <vector> 
#include <iostream>
#include <random>
#include <assert.h>
#include <string>
#include <iomanip>
#include <cmath>
#include <tuple>
#include <algorithm>
#include<windows.h>
#include<unistd.h> 
#include <fcntl.h>
#include <io.h>
using namespace std;


//Symbols for players

// string red_player_symbol = "R", blue_player_symbol = "B", not_played_symbol = "\u2B21", node_link = " \u2014 "; //(1) RED == R, BLUE == B, NOT_PLAYED == Hexagon. Linux users uncomment this line!
string red_player_symbol = "R", blue_player_symbol = "B", not_played_symbol = "*", node_link = " - "; //(1) RED == R, BLUE == B, NOT_PLAYED == Hexagon. Windows users uncomment this line!

//Template for printing vectors
template<typename T>
ostream& operator<<(ostream &os, vector<T> &vec){
    int k = 0;
    for (const auto & element : vec){
        cout<< setprecision(4) << setfill(' ') << setw(6) << element<< ", ";
        if (++k % 7 == 0){ cout<<endl;}
        }
    cout<< endl;
    return os;
}

//Template for calculating argmax in a vector, T must be comparable using <
template<typename T>
int argmax(const vector<T> & vec) {
    if (vec.empty()) {throw invalid_argument("Vector is empty"); }
    return static_cast<int> (distance(vec.begin(), max_element(vec.begin(), vec.end())));
}

//Overloading * operator for strings
string operator*(const int repetitionCount, const string &str) {
    string result;
    for (int i = 0; i < repetitionCount; ++i) {result += str;}
    return result;
}

//Defines Player enumeration to tell who has played which movement, also to define players' colors
enum Player{NO_WINNER= -1, NOT_PLAYED = 0, RED = 1, BLUE = 2};

//Overloads << operator for player enum
ostream& operator<<(ostream &os, const Player &player){
    switch (player){
        case RED:
            cout<<"Red player";
            break;
        case BLUE:
            cout<<"Blue player";
            break;
        case NO_WINNER:
            cout<<"No one has won yet";
            break;
        default:
            break;
    }
    return os;
}

/*HexBoard class:
Implements a hex board, it is quite similar to a graph. It is a simplified graph in order to improve computation and 
memory usage. It contains 5 attributes
    -size: Board's size
    -adjacency_matrix: Determines which nodes(hexagons) are adjacent on the board
    -nodes_to_play: Stores the nodes that have not been claimed by any player
    -DFS_labels: Stores the nodes that have been visited by the Depth-First-Search algorithm, see https://en.wikipedia.org/wiki/Depth-first_search
Methods are explained when they appear*/
class HexBoard {
    private:
        int size;
        vector<vector<int>> adjacency_matrix;
        vector<Player> node_values;
        vector<int> nodes_to_play;
        vector<bool> DFS_labels;
        
    public:
        HexBoard(int size): 
            size(size), 
            adjacency_matrix(vector<vector<int>>(size*size, vector<int>(size*size, 0))),
            node_values(vector<Player>(adjacency_matrix.size(), NOT_PLAYED)), 
            nodes_to_play(vector<int>(adjacency_matrix.size(), 0)),
            DFS_labels(vector<bool>( size*size , false)) {
                initializeHexBoard();
                for(int i = 1; i < size*size; i++){nodes_to_play[i] = i;} //At the begining every node has not been played
            }

        inline int vertices() {//Counts the number of hexagons == size*size 
        return size*size;}

        vector<vector<int>> adjacencyMatrix() { //For debugging purposes
            return adjacency_matrix;}
        
        inline bool adjacent(int u, int v){//Tests if two hexagons are adjacent
            return adjacency_matrix[u][v] > 0;}
        
        inline void setNodeValue(int u, Player value){//Sets the node vale as a player (BLUE or RED) to indicate who possesses that hexagon
            assert(u < vertices() && u >= 0);
            node_values[u] = value;}

        inline Player getNodeValue(int u){//Shows who the hexagon belongs to
            assert(u < vertices() && u >= 0);
            return node_values[u];}
        
        inline vector<int> getNodesToPlay(){//Returns the list of the hexagons not played yet
            return nodes_to_play;}
        
        inline void addSetEdgeValue(int u, int v, int value = 1 ){//Connects the hexagons v and u
            assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
            adjacency_matrix[u][v] = value;}

        inline void resetDFSLabels(){//Resets visited nodes, only for DFS algorithm
            DFS_labels = vector<bool>( size*size , false);}

        inline int deleteNodeToPlay(int node){//Delete one hexagon from the list of non played hexagon i.e. the hexagon was played
            auto index = find(nodes_to_play.begin(), nodes_to_play.end(), node);
            if (index != nodes_to_play.end()){
                nodes_to_play.erase(index);
                return 0;
            }
            cout<< "Unsuccesful deletion\n";
            return 1;
        }

        /*Classifies each node (hexagon) in a Hex board according to its location
        The list is for printing purposes:
        0 - top-right corner 
        1 - top boundary    
        2 - top-left corner
        3 - left boundary
        4 - bottom-left corner
        5 - bottom boundary
        6 - bottom-right corner
        7 - right boundary
        8 - middle 
        */  
        int nodeType(int node){
            int row = node/size, column = node % size;

            if (row == 0){
                if (column == 0){return 0;}
                else if (column == size - 1){return 2;}
                else{return 1;}
                }

            if ( row == size - 1){
                if (column == 0){return 6;}
                else if (column == size - 1){return 4;}
                else {return 5;}
                }
            
            if (column == 0){return 7;}
            else if(column ==  size - 1 ){return 3;}
            else {return 8;}
            }

        void initializeHexBoard(){//Connects the hexagons as the hex board indicates, prior to play
            for (int node = 0; node < vertices(); node++ ){
                switch (nodeType(node)){
                case 0: //top-right corner
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size);
                    break;
                case 1: //top boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size - 1);
                    addSetEdgeValue(node, node + size);
                    break;
                case 2://top-left corner
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node + size - 1);
                    break;
                case 3: //left boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + size - 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    break;
                case 4://bottom-left corner
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node - size);
                    break;
                case 5: //bottom boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node - size);
                    break;
                case 6://bottom-right corner
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size);
                    addSetEdgeValue(node, node - size + 1);
                    break;
                case 7: //right boundary
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    break;
                case 8: //middle
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node + size - 1);
                    break;  
                default:
                    break;
                }
            }
        }

        void printHexBoard(){//Prints the hex board. Please see symbol indications at this code's preamble
            int tab_num = 0;
            printf("Hex board of size %d x %d:\n\n", size, size);
            /*Board symbols, prints two types of line:
            node_line: line with the nodes using characters in (1) preamble
            edge_line: line with the links between nodes using characters in (1) and (2)
            */

            string base_edge_line ="\\ / " , node_line = "0 "; //(2)
            string edge_line =(string)"   " + (size - 1) * base_edge_line + "\\";
            string header_line = "x|y>\nv\\";
            for(int j = 0; j < size; j++){ header_line = header_line + to_string(j) +"   ";}
            cout<<header_line<<endl;
            for (int node = 0; node < vertices(); node++ ){
                if (getNodeValue(node) == RED){node_line += red_player_symbol;}//RED Player played that node
                else if (getNodeValue(node) == BLUE){node_line += blue_player_symbol;}//BLUE Player played that node
                else {node_line += not_played_symbol;}//No one has played that node
                if ( (node % size) == size - 1){
                    //Print lines
                    cout<<node_line<<endl;
                    if (node/size == size-1){break;} //It has printed the last line, no edge line is necessary
                    cout<<edge_line<<endl;
                    //Lines re-initialization (blank node line, double tabbed edge_line)
                    edge_line = 2*(string)" " + edge_line;
                    node_line = "  " + 2*(++tab_num -1)*(string)" " + to_string(node / size + 1 ) + " ";
                    }
                else{node_line +=node_link;}
            }
            cout<<endl;
        }

        vector<int> neighborsColor(int u, Player color){//Shows the list of hexagons neighbouring a given hexagon with the given color
            assert(u < vertices() && (color == RED || color == BLUE));
            vector<int> neighbors_vector;
            for (int j = 0; j < vertices(); j++){
                if (adjacent(u,j) && getNodeValue(j) == color){
                    neighbors_vector.push_back(j);
                }
            }
            return neighbors_vector;
        }

        int executeMovement(Player player, int node, bool print_board = true){//Executes the player's movement in a given node, also it has the option to print the board
            assert(player == RED || player == BLUE);
            if (getNodeValue(node) != NOT_PLAYED){
                cout<<"Node played already, choose another one.\n";
                return 1;}

            if (player == RED){setNodeValue(node, RED);}
            if (player == BLUE){setNodeValue(node, BLUE);}
            deleteNodeToPlay(node);
            if (print_board){
                int row = node/size, column = node % size;
                cout<< player << " did a movement at position ("<< row<<", "<<column <<")\n";
                printHexBoard();
            }
            return 0;
        }

        /*Plays a movement and returns an integer depending whether the movement is legal or not
        Assigns an int to the node_values RED for player 1 and BLUE for player 2, NOT_PLAYED is the default unplayed value*/
        //Here the movement is given as a tuple
        int playMovement(Player player, tuple<int, int> & movement, bool print_board = true){
                int row = get<0>(movement), column = get<1>(movement);
                if ( row < 0 || row >= size || column < 0 || column >= size ){
                    cout<<"Node value outside boundaries, choose another one.\n";
                    return 1;
                }
                int node = (size * row) + column;
                return executeMovement(player, node, print_board);
        }

        //Here the function is overloaded and movement is given as an 0<movement< size*size integer
        int playMovement(Player player, int movement, bool print_board = true){
            if (movement < 0 || movement >= size*size){
                cout<<"Node value outside boundaries, choose another one.\n";
                return 1;
            }
            return executeMovement(player, movement, print_board);
        }
        
        //Executes a variant of the DFS algorithm only going to hexagons with a given color form a given source
        void DFSColor(int source, Player color){
            int row, column;
            assert(source >=0 && source < vertices());
            DFS_labels[source] = true;
            vector<int> neighbors = neighborsColor(source, color);
            for(const auto & neighbor : neighbors){
                if (!DFS_labels[neighbor]){DFSColor(neighbor, color);} //Do DFS recursively
            }
        }

        //Tells the winner if any, or no winner 
        Player winnerChecker(){           
            //Checks if RED won
            for(int source = 0; source < size; source ++){
                if (getNodeValue(source) != RED){continue;}//It's not necessary to do DFS
                resetDFSLabels();
                DFSColor(source, RED);
                for (int k = 0; k < size; k++){
                    if (DFS_labels[ size*(size-1) + k ]){
                        return RED;
                    }
                }
            }
            //Checks if BLUE won
            for(int k = 0, source = 0; k < size; k ++){
                source = k*size;
                if (getNodeValue(source) != BLUE){continue;} //It's not necessary to do DFS
                resetDFSLabels();
                DFSColor(source, BLUE);
                for (int k = 1; k <= size; k++){
                    if (DFS_labels[ k*size - 1]){
                        return BLUE;
                    }
                }
            }
            //No winners
            return NO_WINNER;
        }
};

/*This class implements an AI player that plays using Monte Carlo's method.
Given a hex board assuming it is its turn for each possible movement does 'n_attempts_per_move' random realizations of the entire game.
After it chooses the movement that gave it more winning matches.
It contains two attributes
    -color: A Player enum that indicates which color the AI is playing
    -n_attempts_per_move: Number of random attempts (game realizations) per each feasible move*/
class AIPlayer{
    public:
        Player color;
        int n_attempts_per_move;

        AIPlayer(Player color, int n_attempts_per_move = 100) : 
        color(color),
        n_attempts_per_move(n_attempts_per_move)
        {}

        //Given a hex board fills the entire board with random movements to determine the winner at the end
        void fillBoard(HexBoard & board){
            vector<int> nodes_to_play = board.getNodesToPlay();
            bool your_turn = false;
            Player other_player;
            static random_device rd;
            static mt19937 gen(rd());

            if (color == RED){other_player = BLUE;}
            else {other_player = RED;}

            // Shuffle the vector
            shuffle(nodes_to_play.begin(), nodes_to_play.end(), gen);
            // cout<<nodes_to_play<<"ash\n";
            for ( auto movement : nodes_to_play){
                
                if (your_turn){ 
                    // cout<<movement<<" player "<< color<<endl;
                    board.playMovement(color, movement, false);
                    your_turn = false;}
                else{
                    // cout<<movement<<" player "<< other_player<<endl;
                    board.playMovement(other_player, movement, false);
                    your_turn = true;
                }
            }
            assert(board.getNodesToPlay().size() == 0);
        }

        //The AI generates the next movement after making game realizations per each possible movement
        int nextMovement(HexBoard current_hex_board){
            HexBoard test_board =  current_hex_board;
            HexBoard attempt_board = test_board;
            Player winner;
            vector<int> nodes_to_play = current_hex_board.getNodesToPlay();
            vector<int> counts(nodes_to_play.size(), 0);
            for (int i = 0; i < counts.size() ; i++ ){
                test_board = current_hex_board;
                test_board.playMovement(color, nodes_to_play[i], false );

                for(int attempt = 0; attempt < n_attempts_per_move; attempt++){
                    attempt_board = test_board;
                    fillBoard(attempt_board);
                    winner = attempt_board.winnerChecker();
                    if (winner == color){counts[i] += 1;}
                }
            }
        return nodes_to_play[argmax(counts)];
        }
};

//Plays the Hex game Human vs. AI, the human player can choose their color
void playGameVsAI(int size, int n_attempts){
    int color = -1;
    Player player_color, AI_color;
    bool player_turn;
    //Assigning colors
    while (!(color == 1 || color == 2)){
        cout<< "Select your color. 1 for Red 2 for Blue:\n";
        cin>> color;        
    }
    if (color == 1){
        player_color = RED;
        AI_color = BLUE;
        player_turn = true;
    }
    if (color == 2){
        player_color = BLUE;
        AI_color = RED;
        player_turn = false;
    }
    cout<<"You are the "<<player_color<<"!"<<endl;

    AIPlayer machine_adversary = AIPlayer(AI_color, n_attempts);
    HexBoard board = HexBoard(size);
    int x, y, player_flag, AI_movement;
    tuple<int, int> player_movement;
    //Playing the game
    while (board.winnerChecker() == NO_WINNER){
        if (player_turn){
            player_flag = 1;
            cout<<"Your turn!!\n";
            while (player_flag == 1){
                cout<< "Select x coordinate:"; cin>> x; 
                cout<< "Select y coordinate:"; cin>> y; 
                player_movement = make_tuple(x,y);
                player_flag = board.playMovement(player_color, player_movement);
                player_turn = false;
            }
        }
        else{
            cout<<"AI's turn!!\n";
            AI_movement = machine_adversary.nextMovement(board);
            board.playMovement(AI_color, AI_movement);
            player_turn = true;
        }
    }
    cout << (Player)board.winnerChecker()<< " has won!";
}

void playGameHumanVsHuman(){
    int game_size = -1;
    cout<< "Human vs Human game!!!\nPlease select game's size: ";
    cin >> game_size;
    while (game_size < 1){
        cout << "Select a valid size: ";
        cin >> game_size;
    }

    HexBoard board = HexBoard(game_size);
    int player_turn = 0, x, y, player_flag;
    tuple<int, int> player_movement;
    Player current_player;
    while (board.winnerChecker() == NO_WINNER){
        player_flag = 1;
        if(player_turn % 2 == 0){current_player = RED;}
        else{current_player = BLUE;}
        cout<< current_player <<"'s turn!!\n";
        while (player_flag == 1){
            cout<< "Select x coordinate:"; cin>> x; 
            cout<< "Select y coordinate:"; cin>> y;
            player_movement = make_tuple(x,y);
            player_flag = board.playMovement(current_player, player_movement);
        }
        player_turn++;
    }
    cout << (Player)board.winnerChecker()<< " has won!";
}
    
//Shows and example of the hex grid and its execution. Only for illustration purposes
void showExample(){
    HexBoard hex_board =  HexBoard(3);
    int secondsToSleep = 3;
    
    hex_board.printHexBoard();
    cout<<3*(string)"------------Example-------------";
    cout<<"\n\nHere is an example of a small board with some movements step by step:\n";
    auto movement = make_tuple(1,2); //Pair row column
    hex_board.playMovement(RED, movement);
    sleep(secondsToSleep);
    movement = make_tuple(0,2);
    hex_board.playMovement(BLUE, movement);
    sleep(secondsToSleep);
    movement = make_tuple(2,2);
    hex_board.playMovement(RED, movement);
    sleep(secondsToSleep);
    movement = make_tuple(2,0);
    hex_board.playMovement(BLUE, movement);
    sleep(secondsToSleep);
    movement = make_tuple(0,0);
    hex_board.playMovement(RED, movement );
    sleep(secondsToSleep);
    movement = make_tuple(1,1);
    hex_board.playMovement(BLUE, movement );
    cout<<hex_board.winnerChecker()<<" has won!"<<"\n\n\n\n";
}

int main(){
    
    int example = 0, game_mode = 0 ;
    cout<< "\nThis is a prototype of an implementation of an Hex game board.\n"
    "-In this game there are two players: Blue and Red.\n"
    "-The game's objective is to form a continuous path from one side to another:\n "
    "Red has to connect top and bottom sides, Blue connects left and right sides.\n"
    "-By general rule Red starts first.\n\n"
    "We use some icons to represent each player or a position not used yet\n"
    "-Red player's symbol: " + red_player_symbol + "\n" +
    "-Blue player's symbol: " + blue_player_symbol + "\n" +
    "-Non played's symbol: " + not_played_symbol + " Please go to lines 16 and 17 before executing the code!!!!\n"
    "-The players insert as requested the movements as pairs (x,y), the board has a layout to indicate the position to play."
    "Here there is an empty board example, x goes from top to bottom y goes from left to right.\n\n";
    HexBoard(4).printHexBoard();

    cout<< "Do you want to see an example? (type 1 for yes, type any other key for no) "; 
    cin>> example;
    if (example == 1){showExample();}
        
    int size = 7, n_attempts = 500; //If you want to play in an 11x11 board select 1000 attempts
    
    cout<<"Do you want to play against an AI opponent (type 1) or against another human player (type 2)? "
          "If not, type any other key to quit: ";
    cin>> game_mode;
    if (game_mode == 1){
        cout<< "Now you will play a game against an AI opponent."
        "The variable 'size' considers the hex board size, wheras the 'n_attempts' variable sets up the number of game realizations per movement the AI is allowed to do.\n";
        printf("We set up the size as a borad of %d x %d, and the number of attempts as %d. You can choose your color.\n\n", size, size, n_attempts); 
        playGameVsAI(size, n_attempts);
    }
    else if (game_mode == 2){playGameHumanVsHuman();}
    else {cout<< "Quitting\n";}
    return 0;
}