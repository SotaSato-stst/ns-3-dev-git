import subprocess
from gen_topology import gen
from datetime import datetime
import concurrent.futures
from data import analyze_sim
import yaml

def main():
    yamlData = read_yaml('./sim-variable.yml')

    alphas = yamlData.get('alphas')
    sourceSinkNums = yamlData.get('sourceSinkNums')
    num_nodes = yamlData.get('num_nodes')
    loopCount = yamlData.get('loopCount')
    bandWidth = yamlData.get('bandWidth')

    # 現在の日時を取得
    now = datetime.now()
    # 指定されたフォーマットで日時をフォーマット
    formatted_time = now.strftime("%Y%m%d%H%M%S")
    total_tasks = len(alphas) * len(sourceSinkNums) * loopCount

    with concurrent.futures.ThreadPoolExecutor() as executor:
        futures = []
        for alpha in alphas:
            for sourceSinkNum in sourceSinkNums:
                for i in range(loopCount):
                    fileName = genFileName(formatted_time, alpha, sourceSinkNum, yamlData, i)
                    futures.append(executor.submit(execute_simulation, alpha, sourceSinkNum, fileName, num_nodes, bandWidth))
    completed_tasks = 0
    for future in concurrent.futures.as_completed(futures):
        result = future.result()
        completed_tasks += 1
        print(f"{result} | Completed {completed_tasks}/{total_tasks} tasks")

def execute_simulation(alpha, sourceSinkNum, fileName, num_nodes, bandWidth):
    gen.execute(alpha=alpha, sourceSinkNum=sourceSinkNum, num_nodes=num_nodes, fileName=fileName+".csv")
    subprocess.run(['./ns3', 'run', 'sim.cc', '--', fileName, str(alpha), str(sourceSinkNum), str(bandWidth)])
    analyze_sim.execute(fileName, num_nodes)

def genFileName(time, alpha, sourceSinkNum, yamlData, index):
    def getString(_str, value=None):
        if (value == None):
            value = str(yamlData.get(_str))
        return "_" + _str + "=" + str(value)
    return time + getString('alpha', alpha) + getString("sourceSinkNum", sourceSinkNum) + getString("num_nodes") + getString("bandWidth") + "-" + str(index)

def read_yaml(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        try:
            data = yaml.safe_load(file)  # YAMLファイルを読み込んで辞書として返す
            return data
        except yaml.YAMLError as e:
            print(f"Error reading YAML file: {e}")
            return None

if __name__ == "__main__":
    main()











    # for alpha in alphas:
    #     for sourceSinkNum in sourceSinkNums:
    #         for i in range(loopCount):
    #             fileName = genFileName(formatted_time, alpha, sourceSinkNum, i)
    #             p = multiprocessing.Process(target=execute_simulation, args=(alpha, sourceSinkNum, fileName))
    #             processes.append(p)
    #             p.start()

    # for p in processes:
    #     p.join()

    # Parallel(n_jobs=-1)(
    #     delayed(execute_simulation)(alpha, sourceSinkNum, genFileName(formatted_time, alpha, sourceSinkNum, i))
    #     for alpha in alphas
    #     for sourceSinkNum in sourceSinkNums
    #     for i in range(loopCount)
    # )
