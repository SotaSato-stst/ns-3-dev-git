import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import preferentialAttachment
import calc

# Parameters
num_nodes = 200  # Total number of nodes in the graph
M = 1           # Number of edges to attach from a new node to existing nodes
alpha = 10  # Fraction of links that are updated
sourceSinkNum = 5
leaf_pairs_csv_file_path = './data/leaf_pairs.csv'
adjacency_matrix_csv_file_path = './data/adjacency_matrix.csv'


# Generate the preferential attachment graph
G, adj_matrix = preferentialAttachment.generateGraph(num_nodes, M, alpha)

# Find 3 pairs of leaf nodes
leaf_pairs = calc.find_leaf_pairs(G, num_pairs=sourceSinkNum)

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
