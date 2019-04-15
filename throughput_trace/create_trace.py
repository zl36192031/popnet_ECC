# coding=utf-8

import random
from operator import itemgetter
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

def change_bench_flit(res,flits):
    result = []
    for i in res:
        list1 = [x for x in i .split(" ")]
        list1[5] = flits
        result.append(list1)
    return result

def change_bench_throughput(res,interval,number):
    result = []
    interval1 = 1
    for i in res:
        list1 = [x for x in i.split(" ")]
        interval2 = interval1 + interval
        for i in range(number):
            time = random.randint(interval1,interval2)
            
        interval1 = interval2;
        
def create_random_trace(total,interval,nodes):
    interval1 = 1
    result = []
    for i in range(total):
        interval2 = interval1 + interval
        for j in range(nodes):
            time = random.randint(interval1,interval2)
            sx, sy = random.randint(0,8), random.randint(0,8)
            dx, dy = random.randint(0,8), random.randint(0,8)
            flit = 5
            list1 = [time,sx,sy,dx,dy,flit]
            result.append(list1)
        interval1 = interval2
    return sorted(result, key=itemgetter(0))

def delete_node_trace(res,sx,sy):
    result = []
    for i in res:
        list1 = [x for x in i.split(" ")]
        list1[0] = int(list1[0])
        if int(list1[1]) == sx and int(list1[2]) == sy:
            continue
        result.append(list1)
    return result
        
def change_bench_local_incoming(res,total,interval,number,sx,sy):
    result = delete_node_trace(res,sx,sy)
    interval1 = 1
    for i in range(total):
        interval2 = interval1 + interval
        for j in range(number):
            time = random.randint(interval1,interval2)
            dx, dy = random.randint(0,8), random.randint(0,8)
            flit = 5
            list1 = [time,sx,sy,dx,dy,flit]
            result.append(list1)
        interval1 = interval2
    return sorted(result, key=itemgetter(0))

def create_bench_all_local(total, interval, number):
    result = []
    for sx in range(9):
        for sy in range(9):
            interval1 = 1
            for i in range(total):
                interval2 = interval1 + interval
                for j in range(number):
                    time = random.randint(interval1,interval2)
                    dx, dy = random.randint(0,8), random.randint(0,8)
                    flit = 5
                    list1 = [time,sx,sy,dx,dy,flit]
                    result.append(list1)
                interval1 = interval2                    
    return sorted(result, key=itemgetter(0))
if __name__ == '__main__':
    res = read_bench('bench')
    #result = change_bench_flit(res,5)
    #result = create_random_trace(8000,2,1)
    result = change_bench_local_incoming(res,128000,5,1,2,2)
    #result = create_bench_all_local(4000,256,1)
    writefile(result)
# execcd 
#/path/to/dir$ sh rebench.sh
'''sample ouput 1
'''

