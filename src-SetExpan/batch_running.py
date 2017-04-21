from multiprocessing import Pool
import os,string
import sys

def run(parameter):
    corpus = parameter[0]
    concept = parameter[1]
    max_iter = parameter[2]
    T = parameter[3]
    alpha = parameter[4]
    Q = parameter[5]
    A = parameter[6]
    suffix = "_".join([corpus, concept])

    cmd = "./main -corpus %s -concept %s -suffix %s -max_iter %s -T %s -alpha %s -Q %s -A %s" % (corpus, concept, suffix, max_iter, T, alpha, Q, A)
    os.system(cmd)

if __name__ == '__main__':
    
    # ./main -corpus apr -suffix test -max_iter 10 -T 60 -alpha 0.632 -Q 150 -A 5
    # [-corpus, -max_iter, -T, -alpha, -Q, -A]
    parameters = [
        ["apr", "countries" ,"20", "30", "0.7", "150", "5"],
        ["apr", "laws", "20", "30", "0.7", "150", "5"],
        ["apr", "parties", "20", "30", "0.7", "150", "5"],
        ["apr", "usstates", "20", "30", "0.7", "150", "5"],

        ["wiki", "chinaprovinces", "20", "30", "0.7", "150", "5"],
        ["wiki", "companies", "20", "30", "0.7", "150", "5"],
        ["wiki", "countries", "20", "30", "0.7", "150", "5"],
        ["wiki", "diseases", "20", "30", "0.7", "150", "5"],
        ["wiki", "parties", "20", "30", "0.7", "150", "5"],
        ["wiki", "sportsleagues", "20", "30", "0.7", "150", "5"],
        ["wiki", "usstates", "20", "30", "0.7", "150", "5"],
        ["wiki", "tvchannels", "20", "30", "0.7", "150", "5"],

        ["pubmed_cvd", "hormones", "20", "30", "0.7", "150", "5"]


        # ["wiki", ,"20", "60", "0.7", "150", "5"],
        # ["pubmed_cvd", ,"20", "60", "0.7", "150", "5"]
    ]

    number_of_processes = len(parameters) # 'C'-'Y'

    p = Pool(number_of_processes)
    p.map(run, parameters)