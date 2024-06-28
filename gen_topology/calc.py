import itertools
import random
import numpy as np

def find_leaf_pairs(G, num_pairs=3):
    leaves = [node for node, degree in G.degree() if degree == 1]
    leaf_pairs = list(itertools.combinations(leaves, 2))
    random.shuffle(leaf_pairs)
    return leaf_pairs[:num_pairs]

def calculate_clustering_coefficient(adj_matrix):
    n = len(adj_matrix)
    clustering_coefficients = []

    for i in range(n):
        neighbors = np.where(adj_matrix[i] == 1)[0]
        k_i = len(neighbors)

        if k_i < 2:
            clustering_coefficients.append(0.0)
            continue

        E_i = 0
        for j in range(k_i):
            for k in range(j + 1, k_i):
                if adj_matrix[neighbors[j], neighbors[k]] == 1:
                    E_i += 1

        C_i = (2 * E_i) / (k_i * (k_i - 1))
        clustering_coefficients.append(C_i)

    network_clustering_coefficient = np.mean(clustering_coefficients)
    return network_clustering_coefficient, clustering_coefficients
