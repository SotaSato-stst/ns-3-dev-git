import subprocess
from gen_topology import gen
from datetime import datetime
import concurrent.futures
# from joblib import Parallel, delayed

alphas = [100]
sourceSinkNums = [50]
loopCount = 1

def main():
    # 現在の日時を取得
    now = datetime.now()
    # 指定されたフォーマットで日時をフォーマット
    formatted_time = now.strftime("%Y%m%d%H%M%S")

    with concurrent.futures.ThreadPoolExecutor() as executor:
        for alpha in alphas:
            for sourceSinkNum in sourceSinkNums:
                for i in range(loopCount):
                    fileName = genFileName(formatted_time, alpha, sourceSinkNum, i)
                    execute_simulation(alpha, sourceSinkNum, fileName)

def execute_simulation(alpha, sourceSinkNum, fileName):
    gen.execute(alpha=alpha, sourceSinkNum=sourceSinkNum, fileName=fileName, M=2, num_nodes=200)

def genFileName(time, alpha, sourceSinkNum, index):
    return time + "_" + "alpha=" + str(alpha) + "_" + "sourceSinkNum" + str(sourceSinkNum) + "-" + str(index) + ".csv"
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
