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
        res.append(i.split())
    fp.close()
    return res

def change_bench_hotspot(res):
    result = []
    list2 = [[1,1],[1,4],[1,7],[4,1],[4,4],[4,7],[7,1],[7,4],[7,7]]
    for i in res :
        list1 = [x for x in i.split(" ")]
        des = random.randint(0,0)
        list1[3] = str(list2[des][0])
        list1[4] = str(list2[des][1])
        result.append(list1)
    return result

def change_bench_add(res):
    result = []
    for i in res :
        list1 = [x for x in i.split(" ")]
        s1, s2 = random.randint(0,8), random.randint(0,8)
        d1, d2 = random.randint(0,8), random.randint(0,8)
        result.append(list1)
        result.append([list1[0], str(s1), str(s2), str(d1), str(d2), list1[4]])
    return result

def change_bench_reduce(res):
    result = []
    k = 1
    for i in res :
        list1 = [x for x in i.split(" ")]
        if k % 2 :
            result.append(list1)
        k += 1
    return result 

def change_bench_timespan(res):
    result = []
    k = 1
    for i in res :
        list1 = [x for x in i.split(" ")]
        if k%100 == 1 :
            time1 = list1[0]
        list1[0] = time1
        result.append(list1)
        k += 1
    return result

def change_bench_pathlength(res,length):
    result = []
    for i in res :
        list1 = [x for x in i .split(" ")]
        list2 = []
        for j in range(0,9):
            for k in range(0,9):
                if abs(j-int(list1[1])) + abs(k-int(list1[2])) == length :
                    list2.append([j,k])
        des = list2[random.randint(0,len(list2)-1)]
        list1[3] = str(des[0])
        list1[4] = str(des[1])
        result.append(list1)
    return result

def change_bench_flit(res):
    result = []
    for i in res:
        list1 = [x for x in i .split(" ")]
        list1[5] = "2"
        result.append(list1)
    return result
        
if __name__ == '__main__':
    res = read_bench('bench')
    #result = change_bench_pathlength(res, 6)
    writefile(res)
# exec
#/path/to/dir$ sh rebench.sh
'''sample ouput 1
'''

