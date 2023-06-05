
import math 
import random
import numpy as np
from operator import itemgetter 
from variable import init
ALPHA = 0.6
beta = 0.5
M = 100
e = .001
#e=.001
T0 = []
C = []
S= []
B=[]
num_of_bs = 6
num_of_ue = 1500
weight_cost_2d = init(num_of_ue,num_of_bs); #[[0]*num_of_ue]*num_of_bs



def annealing_algorithm(number, capacity, weight_cost, init_temp,steps = M):
    start_sol = init_solution (weight_cost,capacity)
    best_cost,solution = simulate(start_sol, weight_cost, capacity, init_temp, steps)
    best_combination = [0]*number
    for idx in solution:
        best_combination[idx] = 1
    return best_cost, best_combination
    

def init_solution(weight_cost,max_weight):
    """ Used for initial solution generation.
    By adding a random item while weight is less max_weight
    """
    solution = [] #empty list
    allowed_positions = list(range (len(weight_cost))) # (0,10)
    while len(allowed_positions)>0:
        idx = random.randint(0,len(allowed_positions)-1) # any random number between 0 to 9
        selected_position = allowed_positions.pop(idx)
        if get_cost_and_weight_of_knapsack(solution + [selected_position],weight_cost)[1] <= max_weight:
            solution.append(selected_position)
        else:
            break
    return solution

def get_cost_and_weight_of_knapsack(solution,weight_cost):
    """ Get cost and weight of knapsack - fitness function
    """
    cost,weight = 0,0 
    for item in solution:
        weight += weight_cost[item][0]
        cost += weight_cost[item][1]
    return cost,weight

def moveto (solution, weight_cost, max_weight):
    """All possible moves are genrated"""
    
    moves =[]
    for idx, _ in enumerate (weight_cost): # idx will have index of weight_cost and _ will have exact values
        if idx not in solution:
            move = solution[:]
            move.append(idx)
            
            if get_cost_and_weight_of_knapsack(move,weight_cost)[1] <= max_weight:
                moves.append(move)
    
    for idx, _ in enumerate (solution):
        move = solution [:]
        del move [idx]
        if move not in moves:
            moves.append(move)
    return moves
            

def simulate (solution, weight_cost,max_weight,init_temp,steps):
    temperature = init_temp 
    
    best = solution
    best_cost = get_cost_and_weight_of_knapsack(solution,weight_cost)[0]
    
    current_sol = solution 
    
    while True:
        current_cost = get_cost_and_weight_of_knapsack(best,weight_cost)[0]
        #for i in range (30):
        for i in range (steps):
                 moves = moveto(current_sol,weight_cost,max_weight)
                 idx = random.randint(0,len(moves)-1)
                 random_move = moves[idx]
                 delta = get_cost_and_weight_of_knapsack(random_move,weight_cost)[0] - best_cost
                 if delta > 0:
                     best = random_move
                     best_cost = get_cost_and_weight_of_knapsack(best,weight_cost)[0]
                     current_sol = random_move
                 else:
                      if math.exp(delta/float(temperature)) > random.random():
                            current_sol = random_move
                      #temperature = (600)/(math.log(2+i))
                      #T0.append(temperature)
        
        C.append(current_cost) 
        B.append(best)
        #S.append(steps)
        #steps*=1.1
       # T0.append(temperature)
        temperature *= ALPHA
        if temperature <= e:
            
            break
    return best_cost, best

accepted_user = 0

for i in range(num_of_bs):
    if i==0:
        capacity = 273
    else:
        capacity = 275
    weight_cost = weight_cost_2d[i]
    init_temp = (max(weight_cost,key= itemgetter(1))[1] - min(weight_cost, key = itemgetter(1))[1])*beta 
    best_cost, best_combination = annealing_algorithm(num_of_ue, capacity, weight_cost, init_temp)

    for idx in range(len(best_combination)):
        if best_combination[idx] == 1:
            print("User-Id:",idx," Number of RB:",weight_cost[idx][0]," Throughput:",weight_cost[idx][1])
            accepted_user += 1

    print("Total User:",num_of_ue,"Num of Connected User:",accepted_user, "Churn Rate:",100.0*(1-accepted_user/num_of_ue))
    print("Total throughput:",best_cost)
    # print("Best combination:",best_combination)
