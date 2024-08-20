import pygame
import random
import networkx as nx
from queue import PriorityQueue
from collections import defaultdict
from itertools import combinations, permutations


def chebychev_distance(a, b):
    return max(abs(a[0] - b[0]), abs(a[1] - b[1]))

def taxicab_distance(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

def reconstruct_path(came_from, current):
    total_path = [current]
    while current in came_from.keys():
        current = came_from[current]
        total_path.insert(0, current)
    return total_path


def a_star(G, start, goal, h):
    came_from = {}
    g_score = {node: float('inf') for node in G.nodes}
    g_score[start] = 0
    f_score = {node: float('inf') for node in G.nodes}
    f_score[start] = h(start, goal)

    open_set = PriorityQueue()
    open_set.put((f_score[start], start))
    open_set_hash = set([start])

    while not open_set.empty():
        current = open_set.get()[1]
        open_set_hash.remove(current)

        if current == goal:
            return reconstruct_path(came_from, current)

        for neighbour in G.adj[current]:
            tentative_g_score = g_score[current] + G.edges[current, neighbour]['length']

            if tentative_g_score < g_score[neighbour]:
                came_from[neighbour] = current
                g_score[neighbour] = tentative_g_score
                f_score[neighbour] = tentative_g_score + h(neighbour, goal)

                if neighbour not in open_set_hash:
                    open_set.put((f_score[neighbour], neighbour))
                    open_set_hash.add(neighbour)

    return None


def generate_random_graph():
    nodes = []

    for i in range(grid_width):
        nodes.append((i,i))

    for p in permutations([i for i in range(grid_width)], 2):
        nodes.append(p)

    G = nx.Graph()
    G.add_nodes_from(nodes)

    for edge in combinations(nodes, 2):
        if taxicab_distance(*edge) > 1:
            continue
        length = random.randint(1,5)
        G.add_edge(*edge, length=length)

    return G


pygame.init()
screen = pygame.display.set_mode((800, 800))
clock = pygame.time.Clock()
running = True

grid_width = 50
cell_size = 16

G = generate_random_graph()
a_star_step_generator = a_star(G, list(G.nodes)[0], list(G.nodes)[grid_width-1], chebychev_distance)

step = 0
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill("purple")

    for i, node in enumerate(list(G.nodes)):
        grid_cell = pygame.Rect(node[0]*cell_size, node[1]*cell_size, cell_size, cell_size)
        colour = pygame.Color(200, 200, 200) if (node[0] + node[1]) % 2 == 0 else pygame.Color(100, 100, 100)
        if node == a_star_step_generator[step]:
            colour = pygame.Color(255,0,0)

        screen.fill(colour, grid_cell)

    for edge in G.edges():
        start_pos = (edge[0][0]*cell_size + cell_size//2, edge[0][1]*cell_size + cell_size//2)
        end_pos = (edge[1][0]*cell_size + cell_size//2, edge[1][1]*cell_size + cell_size//2)
        edge_length = G.edges[edge[0], edge[1]]['length']

        pygame.draw.line(screen, pygame.Color(0,255//edge_length,255//edge_length), start_pos, end_pos, 2) 

    pygame.display.flip()

    clock.tick(60)

    step += 1
    if step == len(a_star_step_generator):
        step = 0
        G = generate_random_graph()
        a_star_step_generator = a_star(G, list(G.nodes)[0], list(G.nodes)[grid_width-1], chebychev_distance)

pygame.quit()
