import subprocess
from gen_topology import gen, calc, preferentialAttachment

alphas = [10]
sourceSinkNums = [1]
loopCount = 1

def main():
    for alpha in alphas:
        for sourceSinkNum in sourceSinkNums:
            for _ in range(loopCount):
                gen.execute(alpha=alpha, sourceSinkNum=
                sourceSinkNum)
                subprocess.run(['./ns3', 'run', 'sim.cc', '--', str(alpha), str(sourceSinkNum)])

if __name__ == "__main__":
    main()
