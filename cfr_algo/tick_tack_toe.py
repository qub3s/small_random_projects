import numpy as np
import random
import os

# Node class
class Node:
    counter = 0
    def add_child(self, gamestate):
        self.num_children += 1
        self.childs.append( Node(self.player*(-1), gamestate) )

    def create_childs(self):
        for x in range(3):
            for y in range(3):
                if self.gamestate[x][y] == 0:
                    tmp = np.copy(self.gamestate)
                    tmp[x][y] = self.player
                    self.add_child(tmp)

        return

    def __len__():
        return self.num_children

    def __init__(self, player, gamestate):
        Node.counter += 1
        self.value = eval_board( gamestate )            # The value a specific node has
        self.player = player                            # Int, either 1 or -1
        self.num_children = 0
        self.gamestate = np.copy(gamestate)
        self.childs = []

        if self.value == 0:
            self.create_childs()        

# prints the board (3x3 numpy array)
def p_board( board ):
    print("########")
    for x in range(3):
        print('#', end='')
        for y in board[x]:
            if y == 0:
                print('  ', end='')
            if y == -1:
                print("0 ", end="")
            if y == 1:
                print("X ", end="")
        print('#')
    print("########")

# get the evaluation of a board
def eval_board( board ):
    for x in range(3):
        t = board[:,x]
        if t[0] == t[1] and t[1] == t[2] and t[0] == 1:
            return 1
        if t[0] == t[1] and t[1] == t[2] and t[0] == -1:
            return -1
        
    
    for x in range(3):
        t = board[x]
        if t[0] == t[1] and t[1] == t[2] and t[0] == 1:
            return 1
        if t[0] == t[1] and t[1] == t[2] and t[0] == -1:
            return -1
    
    t = np.fliplr(board).diagonal()
    if t[0] == t[1] and t[1] == t[2] and t[0] == 1:
        return 1
    if t[0] == t[1] and t[1] == t[2] and t[0] == -1:
        return -1

    t = board.diagonal()
    if t[0] == t[1] and t[1] == t[2] and t[0] == 1:
        return 1
    if t[0] == t[1] and t[1] == t[2] and t[0] == -1:
        return -1
    
    return 0

# Calculates recursivly the value of the node
def calc_strat(node):
    if node.num_children == 0:
        return node.value
    
    vs = []     # values of children

    for c in node.childs:
        vs.append( calc_strat(c) )
    
    
    if node.player == -1:
        node.value = min(vs)
    else:
        node.value = max(vs)
    
    return node.value

# translates the input into 2 ints
def get_move(s):
    l = s.split(":")
    x = int(l[0])
    y = int(l[1])

    if x > 2 or x < 0:
        raise ValueError('Fuck you')
    return x,y

# one game iteration
def play_game( r ):
    print("Tick Tack Toe (input x:y | x and y are in the range 0-2)")
    print("0:0  0:1  0:2")
    print("1:0  1:1  1:2")
    print("2:0  2:1  2:2")
    
    while eval_board(r.gamestate) == 0:
        x = input()
        x,y = get_move(x)

        board = r.gamestate.copy()

        if board[x][y] == 0:
            board[x][y] = 1
        else:
            raise ValueError('Fuck you') 
        
        for x in r.childs:
            if np.array_equal(x.gamestate,board):
                r = x
        
        if r.num_children == 0:
            break

        l = []
        b = True
        for x in r.childs:
            if x.value == -1:
                r = x
                b = False
            elif x.value == 0:
                l.append(x)
        
        if b:
            r = random.choice(l)

        p_board(r.gamestate)

    if eval_board(r.gamestate) != 0:
        if eval_board(r.gamestate) < 0:
            print("Lost")
        else:
            print("Won")
    else:
        print("Draw")
    p_board(r.gamestate)



board = np.zeros((3, 3))
root = Node(1,board)
calc_strat(root)

t = "y"

while t == "y":
    play_game(root)

    t = input("Want to play again (y/n)")
    os.system('clear')
