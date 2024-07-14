import subprocess
from gen_topology import gen
from datetime import datetime
import concurrent.futures
from data import analyze_sim
# from joblib import Parallel, delayed

alphas = [0]
sourceSinkNums = [5]
num_nodes = 100
loopCount = 1

def main():
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
                    fileName = genFileName(formatted_time, alpha, sourceSinkNum, i)
                    futures.append(executor.submit(execute_simulation, alpha, sourceSinkNum, fileName))

    completed_tasks = 0
    for future in concurrent.futures.as_completed(futures):
        result = future.result()
        completed_tasks += 1
        print(f"{result} | Completed {completed_tasks}/{total_tasks} tasks")

def execute_simulation(alpha, sourceSinkNum, fileName):
    gen.execute(alpha=alpha, sourceSinkNum=sourceSinkNum, num_nodes=num_nodes, fileName=fileName+".csv")
    subprocess.run(['./ns3', 'run', 'sim.cc', '--', fileName, str(alpha), str(sourceSinkNum)])
    analyze_sim.execute(fileName, num_nodes)

def genFileName(time, alpha, sourceSinkNum, index):
    return time + "_" + "alpha=" + str(alpha) + "_" + "sourceSinkNum" + str(sourceSinkNum) + "-" + str(index)

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
