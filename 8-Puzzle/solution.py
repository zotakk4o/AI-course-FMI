import math
import numpy as np
from queue import PriorityQueue

#the opposite direction of the free tile
directions_dictionary = ['right', 'left', 'down', 'up']
visited = []

class State:
    def __init__(self, tiles, free_tile_location = None, g=0):
        self.tiles = tiles

        self.g = g
        self.h = 0
        self.children = PriorityQueue()
        self.parent = None
        self.free_tile_location = self.find_free_tile() if free_tile_location is None else free_tile_location

    def __eq__(self, other):
        return np.allclose(self.tiles, other.tiles)

    def __lt__(self, other):
        return self.f < other.f

    def __le__(self, other):
        return self.f <= other.f

    def __gt__(self, other):
        return self.f > other.f

    def __ge__(self, other):
        return self.f >= other.f

    @property
    def f(self):
        return self.g + self.h

    def to_array(self):
        return [col for row in self.tiles for col in row]

    def to_matrix(self):
        return self.tiles

    def find_free_tile(self):
        size = len(self.tiles)
        for i in range(size):
            for j in range(size):
                if self.tiles[i][j] == 0:
                    return (i, j)

    def generate_children(self, free_tile_target):
        x, y = self.free_tile_location
        #direction of free tile
        directions = [[x, y - 1], [x, y + 1], [x - 1, y], [x + 1, y]]
        size = len(self.tiles)
        for i, dir in enumerate(directions):
            if dir[0] < 0 or dir[0] > size - 1 or dir[1] < 0 or dir[1] > size - 1:
                continue
            self.tiles[x][y], self.tiles[dir[0]][dir[1]] = self.tiles[dir[0]][dir[1]], self.tiles[x][y]
            state = State(np.copy(self.tiles), dir, self.g + 1)
            state.compute_heuristics_manhattan(free_tile_target[0] * size + free_tile_target[1])
            state.parent = (self, directions_dictionary[i])
            self.children.put(state)
            self.tiles[x][y], self.tiles[dir[0]][dir[1]] = self.tiles[dir[0]][dir[1]], self.tiles[x][y]

    def compute_heuristics_manhattan(self, free_tile_index):
        state = self.tiles
        size = len(state)
        heuristics = 0
        for i in range(size):
            for j in range(size):
                num = state[i][j]
                if num != 0:
                    if num < free_tile_index:
                        num -= 1

                    num_real_row = num // size
                    num_real_col = num % size
                else:
                    num_real_row = free_tile_index // size
                    num_real_col = free_tile_index % size
                heuristics += abs(i - num_real_row) + abs(j - num_real_col)

        self.h = heuristics

def has_solution(arr_from_tiles):
    inversions_count = 0
    for i in range(0, len(arr_from_tiles)):
        for j in range(i + 1, len(arr_from_tiles)):
            if arr_from_tiles[i] and arr_from_tiles[j] and arr_from_tiles[i] > arr_from_tiles[j]:
                inversions_count += 1

    return inversions_count % 2 == 0

def get_final_state(tiles, free_tile_index):
    res_array = []
    offset = 1
    for i in range(0, len(tiles)**2) :
        if i == free_tile_index:
            res_array.append(0)
            offset -= 1
        else:
            res_array.append(i + offset)

    res = np.array(res_array).reshape(size, size)
    return res

def a_star(current, goal, limit):
    if current == goal:
        return current

    if limit == 0:
        return False

    visited.append(current)
    current.generate_children(goal.free_tile_location)
    while not current.children.empty():
        child = current.children.get()
        if child not in visited:
            #print(child.tiles)
            #print(limit)
            res = a_star(child, goal, limit - 1)
            if res:
                return res

    return False


def print_result(steps, final_state):
    print(steps)
    res = []
    while final_state.parent:
        res.append(final_state.parent[1])
        final_state = final_state.parent[0]
    res.reverse()
    print("\n".join(res))

def iterative_deepening_a_star(start, goal, limit):
    for l in range(1, limit + 1):
        print("L: ", l)
        visited.clear()
        res = a_star(start, goal, l)
        if res:
            print_result(l, res)
            return

n = int(input())
size = int(math.sqrt(n + 1))
if size * size != n + 1:
    raise AssertionError("Wrong size!")

free_tile_index = int(input())
free_tile_index = free_tile_index if free_tile_index != -1 else n
tiles = []
for r in range(size):
    row = list(map(lambda x: int(x), input().split(' ')))
    tiles.append(row)

start_state = State(tiles)

if not has_solution(start_state.to_array()):
    print("Puzzle not solvable!")
    exit(0)

start_state.compute_heuristics_manhattan(free_tile_index)
final_state = State(get_final_state(tiles, free_tile_index), (free_tile_index // size, free_tile_index % size))
print("FINAL STATE")
print(final_state.tiles)
iterative_deepening_a_star(start_state, final_state, 50)