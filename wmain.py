import numpy as np
import networkx as nx
import cPickle as cp
import random
import ctypes
import os
import sys
from tqdm import tqdm
from scipy.sparse import csr_matrix
from scipy.io import mmread

sys.path.append( '%s/verel_lib' % os.path.dirname(os.path.realpath(__file__)) )
from verel_lib import VerelLib

def PrepareGraphs(g, isTest):
    if not isTest:
        api.ClearTrainGraphs()
    api.InsertGraph(g, is_test=isTest)

if __name__ == '__main__':
    api = VerelLib(sys.argv)
    
    B = mmread('bcsstk01.mtx');

    g = nx.from_scipy_sparse_matrix(B)

    opt = {}
    for i in range(1, len(sys.argv), 2):
        opt[sys.argv[i][1:]] = sys.argv[i + 1]

    PrepareGraphs(g, isTest=True)
    PrepareGraphs(g, isTest=False)

    # startup    
    for i in range(10):
        api.lib.PlayGame(100, ctypes.c_double(1.0))
    api.TakeSnapshot()

    eps_start = 1.0
    eps_end = 1.0

    eps_step = 10000.0
#    api.lib.SetSign(1)

    lr = float(opt['learning_rate'])
    for iter in range(int(opt['max_iter'])):
        eps = eps_end + max(0., (eps_start - eps_end) * (eps_step - iter) / eps_step)
        if iter % 10 == 0:
            api.lib.PlayGame(10, ctypes.c_double(eps))

        if iter % 100 == 0:
            frac = api.lib.Test(0)
            print 'iter', iter, 'lr', lr, 'eps', eps, 'average number of new edges: ', frac
            sys.stdout.flush()
#            model_path = '%s/%s_iter_%d.model' % (opt['save_dir'], opt['sample_name'], iter)
#            api.SaveModel(model_path)

#        if iter % 1000 == 0:
#            api.TakeSnapshot()
#            lr = lr * 0.95

        api.lib.Fit(ctypes.c_double(lr))
