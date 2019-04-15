# coding=utf-8

import random
def readConfig(fname):
    if type(fname) != str:
        print fname, 'is not str'
        return
    res = []
    fp = open(fname, 'r')
    for i in fp:
        res.append(i.replace("\n", ""))
    fp.close()
    return res

def writefile(res):
    if type(res) != list:
        print 'ERROR TYPE OF res'
        return
    fp = open('bench', 'w')
    print "\nthe result of bench:"
    for i in res:
        # transfer list such as (20.0, 15, 12, 9, 4, 5) to str
        line = i[0], i[1], i[2], i[3],i[4],i[5]
        strline = str(line[0])
        j = 1
        while j < len(line):
            strline = strline + ' ' + str(line[j])
            j += 1
        print strline
        fp.write(strline + "\n")
    fp.close
    # create bench.x.x files
    benchxxs = set()
    fp = open('bench', 'r')
    benchs = []
    for i in fp.readlines():
        line = i.split(' ')
        benchxx = "bench." + str(line[1]) + "." + str(line[2])
        fpxx = open(benchxx, 'a')
        fpxx.write(i)
        fpxx.close()
    fp.close()

def read_bench(fname):
    res = []
    fp = open(fname, 'r')
    for i in fp :
        res.append(i.replace("\n", ""))
    fp.close()
    return res

def change_bench_local(res):
    result = []
    for i in res :
        list1 = [x for x in i.split(" ")]
        if int(list1[1]) == 0 :
            a = random.randint(0, 1)
        elif int(list1[1]) == 8 :
            a = random.randint(-1, 0)
        else :
            a = random.randint(-1, 1)
        if int(list1[2]) == 0 :
            b = random.randint(0, 1)
        elif int(list1[2]) == 8 :
            b = random.randint(-1, 0)
        else :
            b = random.randint(-1, 1)
        list1[3] = str(int(list1[1]) + a)
        list1[4] = str(int(list1[2]) + b)
        result.append(list1)
    return result

if __name__ == '__main__':
    res = read_bench('bench')
    result = change_bench_local(res)
    writefile(result)
# exec
#/path/to/dir$ sh rebench.sh
'''sample ouput 1
'''

