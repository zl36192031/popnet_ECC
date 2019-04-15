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

def change_bench_hotspot(res):
    result = []
    for i in res :
        list1 = [x for x in i.split(" ")]
        a = random.randint(0,1)
        if a == 0 :
            list1[1] = '4'
            list1[2] = '4'
        else :
            list1[3] = '4'
            list1[4] = '4'
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

if __name__ == '__main__':
    res = read_bench('bench')
    result = change_bench_reduce(res)
    writefile(result)
# exec
#/path/to/dir$ sh rebench.sh
'''sample ouput 1
'''

