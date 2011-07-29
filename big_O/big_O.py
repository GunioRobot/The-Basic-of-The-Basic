from timeit import Timer
import numpy
import math

def constant(nlist, tlist):
    n = numpy.ones((len(nlist), 1))
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]
    
def double_log(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), numpy.log(numpy.log(nlist))]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

def logarithmic(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), numpy.log(nlist)]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

#Todo
#def frac_power(nlist, tlist):

def linear(nlist, tlist):
    #transpose
    n = numpy.vstack([numpy.ones(len(nlist)), nlist]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

def linearithmic(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), nlist * numpy.log(nlist)]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

def quadratic(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), nlist * nlist]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

def polynomial(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), numpy.log(nlist)]).T
    t = numpy.log(tlist)
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

def exponential(nlist, tlist):
    n = numpy.vstack([numpy.ones(len(nlist)), nlist]).T
    t = numpy.log(tlist)
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]

#Todo:
#def sub_exponential(nlist, tlist):
"""
def factorial(nlist, tlist):
    list = []
    for i in nlist:
        list.append(math.factorial(i))
    n = numpy.vstack([numpy.ones(len(nlist)), list]).T
    t = tlist
    x, resids, rank, s = numpy.linalg.lstsq(n, t)
    #return sum of square errors
    return resids[0]
"""
def exe_time(func_name, min_num, max_num):
    """Execute the function.
        
        -------------------------------------------------------
        exe_time(func_name, min_num, max_num):
        -------------------------------------------------------
        Input:
        func_name   --  the function the complexity of whic is
                        going to be measured. The function must
                        only take one integer as parameter.
        min_num     --  min possible value for the parameter.
        max_num     --  max possible value for the parameter.
        -------------------------------------------------------
        Output:
        nlist       --  a list of integers was tried
        tlist       --  a list of execution time for each 
                        integer in nlist
        -------------------------------------------------------
        """
    class wrapper(object):
        def __init__(self, n):
            self.para = n
        def __call__(self):
            return func_name(self.para)
    
    nlist = numpy.linspace(min_num, max_num, 10).astype('int64')
    tlist = numpy.empty(10)
    for i, n in enumerate(nlist):
        timer = Timer(wrapper(n))
        tlist[i] = timer.timeit(10000)
    return nlist, tlist

def get_result(nlist, tlist):
    """Get the estimated big O complexity category.
        
        -------------------------------------------------------
        get_result(nlist, tlist):
        -------------------------------------------------------
        Input:
        nlist       --  a list of integers was tried
        tlist       --  a list of execution time for each 
                        integer in nlist
        -------------------------------------------------------
        Output:
        info        --  a string of estimated result, including
                        a best fit category for big O notation 
        -------------------------------------------------------
        """
    info = ":("
    #positive infinity
    choice = 1e308
    
    temp = constant(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Constant"

    temp = double_log(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Double Logarithmic"

    temp = logarithmic(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Logarithmic"

    temp = linear(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Linear"

    temp = linearithmic(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Linearithmic"

    temp = quadratic(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Quadratic"

    temp = polynomial(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Polynomial"

    temp = exponential(nlist, tlist)
    if temp < choice:
        choice = temp
        info = "Exponential"

            #    temp = factorial(nlist, tlist)
            #    if temp < choice:
            #        choice = temp
            #        info = "Fractorial"    
    
    return info


def big_O(func_name, min_num, max_num):
    """Estimate big O notation complexity of a given class
        based on its execution time. See analysis of algorithms
        on wikipedia to know the drawback of it.
    
        -------------------------------------------------------
        big_O(func_name, min_num, max_num):
        -------------------------------------------------------
        Input:
        func_name   --  the function the complexity of whic is
                        going to be measured. The function must
                        only take one integer as parameter.
        min_num     --  min possible value for the parameter.
        max_num     --  max possible value for the parameter.
        -------------------------------------------------------
        Output:
        info        --  a string of estimated result, including
                        a best fit category for big O notation
        -------------------------------------------------------
        """
    nlist, tlist = exe_time(func_name, min_num, max_num)
    return get_result(nlist, tlist)