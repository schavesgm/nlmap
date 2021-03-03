import os, regex
from collections import namedtuple

__all__ = [
    'clean_params', 'clean_residues', 
    'clean_sigma', 'clean_resparams'
]


# Namedtuple containing the results from buccaneer parameters
Params = namedtuple(
    'params', ('Rw', 'Rf', 'L', 'A', 'V')
)

# Namedtuple containing the results from buccaneer residues
Residues = namedtuple(
    'residues', ('resb', 'prun', 'chain', 'cbuild', 'cchain')
)

# Clean the parameters
def clean_params(result_list):
    ''' Get the parameters from a parameters list. '''

    # Get the result from the list
    Rw_str, Rf_str, L_str, A_str, V_str = result_list[1:6]

    # Regex the strings to obtain the results
    Rw = [float(f) for f in regex.findall('\d+.\d+', Rw_str)]
    Rf = [float(f) for f in regex.findall('\d+.\d+', Rf_str)]
    L  = [float(f) for f in regex.findall('\d+.\d+', L_str)]
    A  = [float(f) for f in regex.findall('\d+.\d+', A_str)]
    V  = [float(f) for f in regex.findall('\d+.\d+', V_str)]

    return Params(Rw, Rf, L, A, V)

# Clean the residues
def clean_residues(result_list):
    ''' Get the residues from a residues list. '''

    # Regex the strings in the list to obtain the results
    resb   = [int(f) for f in regex.findall('\d+', result_list[0])]
    prun   = [int(f) for f in regex.findall('\d+', result_list[1])]
    chain  = [int(f) for f in regex.findall('\d+', result_list[2])]
    cbuild = [float(f) for f in regex.findall('\d+.\d+', result_list[3])]
    cchain = [float(f) for f in regex.findall('\d+.\d+', result_list[4])]

    return Residues(resb, prun, chain, cbuild, cchain)

# Clean the sigma values
def clean_sigma(list_results):
    ''' Extract the sigma parameters from a list of results. '''

    # Import numpy
    import numpy as np

    # List containing the iterations and sigma examples
    it_list = [f for f in list_results if 'Iteration Number' in f]
    ex_list = [f for f in list_results if regex.match('\d+.\d+--\s?\d+.\d+', f)]

    # Count the number osf cycles in the list
    n_cycles, examples = len(it_list) + 1, int(len(ex_list) / 3)

    # Generate the numpy array that will contain the sigma results
    sigma = np.empty([examples, n_cycles + 2])

    # Counter of number of cycles used
    cycle = 2

    # Clean the data from the example list
    for idx, example in enumerate(ex_list):

        # Bound idx to number of examples
        idx = idx % examples

        # Strip the example
        example = example.strip()

        # Regex the data in the example
        matches = regex.match('(\d+.\d+)--\s?(\d+.\d+)\s+(\d+.\d+).*', example)

        sigma[idx, 0] = matches.group(1).replace(' ', '')
        sigma[idx, 1] = matches.group(2).replace(' ', '')
        sigma[idx, cycle] = matches.group(3)

        if idx == examples - 1:
            cycle += 1

    return sigma

# Clean all parameters and residues inside two lists
def clean_resparams(list_params, list_resids):
    ''' Clean all the parameters and residues in a list. '''

    # Lists containing the cleaned results
    cleaned_params, cleaned_resids = [], []

    # Clean the data from the parameters
    for params in list_params:
        cleaned_params.append(clean_params(params))

    # Clean the data from the residues
    for resids in list_resids:
        cleaned_resids.append(clean_residues(resids))

    return cleaned_params, cleaned_resids

if __name__ == '__main__':
    pass

