from turtle import update
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import itertools
import random

# Parameters
num_nodes = 200  # Total number of nodes in the graph
M = 1           # Number of edges to attach from a new node to existing nodes
alpha = 10  # Fraction of links that are updated
sourceSinkNum = 5
leaf_pairs_csv_file_path = './data/leaf_pairs.csv'
adjacency_matrix_csv_file_path = './data/adjacency_matrix.csv'


def preferential_attachment_graph(num_nodes, M, alpha):
    G = nx.complete_graph(M + 1)

    for new_node in range(M + 1, num_nodes):
        NodeNum = len(G.nodes())
        degrees = np.array([G.degree(n) for n in G.nodes()])

        # 確率式: https://arxiv.org/pdf/cond-mat/0011442
        probs = (degrees + (alpha * M)) / ((1 + alpha) * M * NodeNum)
        # 正規化
        total_probs = np.sum(probs)
        probs = probs / total_probs

        # probs = probs * ((NodeNum * M * (1+alpha)) /
        #                  ((NodeNum * alpha * M) + 2 * (NodeNum - 1)))
        existing_nodes = np.random.choice(
            G.nodes(), size=M, replace=False, p=probs)

        for existing_node in existing_nodes:
            G.add_edge(new_node, existing_node)

    adj_matrix = nx.to_numpy_array(G)

    # シミュレーションように上三角部分の1を対象部分に移し、下三角行列にする
    for i in range(adj_matrix.shape[0]):
        for j in range(i + 1, adj_matrix.shape[1]):
            if adj_matrix[i, j] == 1:
                adj_matrix[i, j] = 0
                adj_matrix[j, i] = 1
    return G, adj_matrix


def find_leaf_pairs(G, num_pairs=3):
    leaves = [node for node, degree in G.degree() if degree == 1]
    leaf_pairs = list(itertools.combinations(leaves, 2))
    random.shuffle(leaf_pairs)
    return leaf_pairs[:num_pairs]


# Generate the preferential attachment graph
G, adj_matrix = preferential_attachment_graph(num_nodes, M, alpha)

# Find 3 pairs of leaf nodes
leaf_pairs = find_leaf_pairs(G, num_pairs=sourceSinkNum)

# Save the leaf pairs to a CSV file
leaf_pairs_df = pd.DataFrame(leaf_pairs)
leaf_pairs_df.to_csv(leaf_pairs_csv_file_path, index=False, header=False)

# Print the path to the CSV file
print(f"Leaf pairs saved to {leaf_pairs_csv_file_path}")

# Save the adjacency matrix to a CSV file
df = pd.DataFrame(adj_matrix)
df.to_csv(adjacency_matrix_csv_file_path, index=False, header=False)

# Print the path to the CSV file
print(f"Adjacency matrix saved to {adjacency_matrix_csv_file_path}")

# Optionally, visualize the graph
G = nx.from_numpy_array(adj_matrix)
plt.figure(figsize=(7, 10))
nx.draw(G, node_size=20, node_color='blue',
        edge_color='gray', with_labels=False)
plt.show()
