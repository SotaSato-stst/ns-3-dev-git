import networkx as nx
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import numpy as np
from . import analyze_eachnode_packet_process
# import analyze_eachnode_packet_process
import csv

packet_info_result_dir_path = './data/result/packet_info/'

def execute(fileName, num_nodes, alpha, sourceSinkNum, isDeleteFile=True):
    adjacent_matrix = np.genfromtxt(getAdjacencyMetrixPath(fileName), delimiter=',', dtype=float)
    plotNetworkWithPacketProcessAmount(fileName, adjacent_matrix, num_nodes, alpha, sourceSinkNum)
    if isDeleteFile:
        deleteFile(fileName)

def plotNetworkWithPacketProcessAmount(fileName, adjacent_matrix, num_nodes, alpha, sourceSinkNum):
    node_counts = analyze_eachnode_packet_process.execute(getAsciiFIlePath(fileName), num_nodes)
    G = nx.from_numpy_array(adjacent_matrix)
    betweenness_centrality = nx.betweenness_centrality(G).values()
    os.makedirs(f"./data/output/networkplot/{fileName}", exist_ok=True)
    os.makedirs(f"./data/output/scatter/{fileName}", exist_ok=True)
    noderank = list(range(num_nodes))

    enqueue_counts = [node_counts[key][0] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, enqueue_counts, "enqueue_counts")
    plotScatter(fileName, noderank, sorted(enqueue_counts, reverse=True), "NodeRank", "Count", "enqueue_counts", "enqueue_counts")

    dequeue_counts = [node_counts[key][1] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, dequeue_counts, "dequeue_counts")
    plotScatter(fileName, noderank, sorted(dequeue_counts, reverse=True), "NodeRank", "Count", "dequeue_counts", "dequeue_counts")

    loss_counts = [node_counts[key][2] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, loss_counts, "loss_counts")
    plotScatter(fileName, noderank, sorted(loss_counts, reverse=True), "NodeRank", "Count", "loss_counts", "loss_counts")

    receive_counts = [node_counts[key][3] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, receive_counts, "receive_counts")
    plotScatter(fileName, noderank, sorted(receive_counts, reverse=True), "NodeRank", "Count", "receive_counts", "receive_counts")

    plotNetworkWithWeigh(fileName, G, betweenness_centrality, "betweenness_centrality")
    plotScatter(fileName, noderank, sorted(betweenness_centrality, reverse=True), "NodeRank", "Count", "betweenness_centrality", "betweenness_centrality")

    total_enqueue_counts = sum(enqueue_counts)
    total_dequeue_counts = sum(dequeue_counts)
    total_loss_counts = sum(loss_counts)
    total_receive_counts = sum(receive_counts)
    packet_loss_rate = total_loss_counts / total_enqueue_counts

    with open(packet_info_result_dir_path + fileName + ".csv", mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([alpha, sourceSinkNum, total_enqueue_counts, total_dequeue_counts, total_loss_counts, total_receive_counts, packet_loss_rate])

def plotNetworkWithWeigh(metadata, G, node_weights, node_weights_type):
    cmap = plt.get_cmap('cool')  # 例として'cool'カラーマップを使用

    # 各ノードの色を大きさに応じて設定する
    max_weight =max(node_weights)
    if max_weight == 0:
        max_weight = 1
    node_colors = [cmap(size / max_weight) for size in node_weights]
    plt.figure(figsize=(7, 10))
    np.random.seed(10)
    nx.draw(G, edge_color='gray', node_color=node_colors, node_size=40, with_labels=False)

    filename = f'{node_weights_type}.png'
    filepath = os.path.join(f"./data/output/networkplot/{metadata}", filename)
    plt.savefig(filepath)
    plt.close()

def plotScatter(metadata, x, y, x_label, y_label, scatterType, title="scatter"):
    # 散布図を描画する
    plt.scatter(x, y, s=10)

    # グラフのタイトルと軸ラベルを設定する
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.ylim(0, 1000)
    filename = f'{scatterType}.png'
    filepath = os.path.join(f"./data/output/scatter/{metadata}", filename)
    plt.savefig(filepath)
    plt.close()

def deleteFile(fileName):
    os.remove(getAsciiFIlePath(fileName))


def getAsciiFIlePath(fileName):
    return './data/raw/ascii/' + fileName + '.tr'

def getAdjacencyMetrixPath(fileName):
    return './data/topology_data/adjacency_matrix/' + fileName + '.csv'

if __name__ == "__main__":
    execute('20240715195202_alpha=0_sourceSinkNum5-0', 100, isDeleteFile=False)
