from turtle import update
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt


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
    return adj_matrix


# Parameters
num_nodes = 200  # Total number of nodes in the graph
M = 1           # Number of edges to attach from a new node to existing nodes
alpha = 0.3  # Fraction of links that are updated

# Generate the preferential attachment graph
adj_matrix = preferential_attachment_graph(num_nodes, M, alpha)

# Print the adjacency matrix
print(adj_matrix)

# Optionally, visualize the graph
G = nx.from_numpy_array(adj_matrix)
plt.figure(figsize=(8, 8))
nx.draw(G, node_size=50, node_color='blue',
        edge_color='gray', with_labels=False)
plt.show()
