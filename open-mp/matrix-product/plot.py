
import matplotlib.pyplot as plt

def getTimes(tgtlines):
    numvec=[]

    for line in tgtlines:
        target=line
        mintime=''
        sectime=''
        counter=0
        for char in target:
            if char=='m':
                break
            else:
                counter+=1
                mintime+=char
        for i in range(counter+1, len(target)):
            if target[i]=='s':
                break
            else:
                sectime+=target[i]
        if ',' in sectime:
            sectime = sectime.replace(',','.')
        numsec=float(sectime)
        nummin=int(mintime)
        rtime=60*nummin+numsec
        numvec.append(rtime)
    return numvec

def main():
    files = {'t1.txt':[], 't2.txt':[], 't3.txt':[]}
    threads = [1,2,4,8,16,32,64,128]

    fig, axes = plt.subplots(ncols=1, nrows=3, sharex=True, figsize=(30,10))

    for index, file in enumerate(files.keys()):

      with open(file, 'r') as file1:
        lines = file1.readlines()
        file1.close()
        reallines=[]
        for i, line in enumerate(lines):
              if 'real' in line:
                  reallines.append(line.split()[1])
              files[file] = getTimes(reallines)

      axes[index].set_title(f"Program {file.replace('.txt','')}")
      axes[index].plot(threads, files[file])
      axes[index].axvline(8, color='r', linestyle='-.')
      axes[index].set_xlabel('N Threads')
      axes[index].set_ylabel('Time [s]')

    plt.show()


if __name__=='__main__':
  main()
