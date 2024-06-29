import itertools
import random
import numpy as np

def find_leaf_pairs(G, num_pairs=3):
    leaves = [node for node, degree in G.degree() if degree == 1]
    leaf_pairs = list(itertools.combinations(leaves, 2))
    random.shuffle(leaf_pairs)
    return leaf_pairs[:num_pairs]

def calculate_clustering_coefficient(adj_matrix):
    symmetric_matrix = make_symmetric(adj_matrix)

    n = len(symmetric_matrix)
    clustering_coefficients = []
    print(symmetric_matrix)
    for i in range(n):
        neighbors = np.where(symmetric_matrix[i] == 1)[0]
        k_i = len(neighbors)

        print(neighbors)


        if k_i < 2:
            clustering_coefficients.append(0.0)
            continue

        E_i = 0
        for j in range(k_i):
            for k in range(j + 1, k_i):
                if symmetric_matrix[neighbors[j], neighbors[k]] == 1:
                    E_i += 1

        C_i = (2 * E_i) / (k_i * (k_i - 1))
        clustering_coefficients.append(C_i)

    network_clustering_coefficient = np.mean(clustering_coefficients)
    return network_clustering_coefficient

def make_symmetric(matrix):
    n = matrix.shape[0]  # Assume the matrix is square
    for i in range(n):
        for j in range(i):
            if matrix[i, j] == 1:
                matrix[j, i] = 1
    return matrix
