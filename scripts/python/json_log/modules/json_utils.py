import os
import regex
import json
import numpy

# Needs OrderedDict for older python versions
from collections import OrderedDict

from .parse_utils import parse_refmtz
from .parse_utils import parse_map

__all__ = ['pipeline_JSON']

# Simulation name string format
SIMULATION_FORMAT = 's(\d+.\d+)_h(\d+.\d+)_r(\d+.\d+)_p(\d+.\d+)_e(\d+.\d+)'

# Custom JSON encoder and container to serialise data {{{
class InlineList:
    def __init__(self, l):
        self._list = l

class InlineDict:
    def __init__(self, d):
        self._dict = d

class CustomEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, InlineList):
            return "##<{}>##".format(obj._list)
        if isinstance(obj, InlineDict):
            return '##<{}>##'.format(obj._dict)
        if isinstance(obj, numpy.ndarray):
            return obj.tolist() 
        return json.JSONEncoder.default(self, obj)
# }}}

# Dump the reference mtz and the map files content {{{
def dump_refmtz(refmtz_content):
    ''' Dump the contents of the reftmtz build. '''
    return OrderedDict([
        ('params'  , dump_params(refmtz_content['params'])),
        ('residues', dump_residues(refmtz_content['resids']))
    ])

def dump_map(map_content):
    ''' Dump the contents of a map build. '''
    return OrderedDict([
        ('params'  , dump_params(map_content['params'])),
        ('residues', dump_residues(map_content['resids'])),
        ('sigma'   , dump_sigma(map_content['sigma']))
    ])
# }}}

# Functions to convert the content dictionaries to good format {{{
def param_split(key, result_obj):
    ''' 
    Generate a dictionary containing the initial and
    final parameters.
    '''
    return OrderedDict([
        ('o', InlineList([getattr(p, key)[0] for p in result_obj])),
        ('f', InlineList([getattr(p, key)[1] for p in result_obj])),
    ])

def dump_residues(residues_dict):
    '''
        Generate a dictionary containing the organised version
        of the residues data.
    '''
    return OrderedDict([
        ('resb'      , InlineList([p.resb for p in residues_dict])),
        ('prunning'  , InlineList([p.prun[0] for p in residues_dict])),
        ('chains'    , InlineList([p.chain for p in residues_dict])),
        ('comp_build', InlineList([p.cbuild[0] for p in residues_dict])),
        ('comp_chain', InlineList([p.cchain[0] for p in residues_dict])),
    ])

def dump_params(params_dict):
    '''
        Generate a dictionary containing the parameter results.
    '''
    return OrderedDict([
        ('Rw'   , param_split('Rw', params_dict)),
        ('Rf'   , param_split('Rf', params_dict)),
        ('rms_L', param_split('L',  params_dict)),
        ('rms_A', param_split('A',  params_dict)),
        ('rms_V', param_split('A',  params_dict)),
    ])

def dump_sigma(sigma_array):
    '''
        Generate a dictionary containing the sigma results.
    '''

    # Dictionary containing the final results
    results = OrderedDict()

    # Dump each value to the dictionary
    for row in sigma_array:

        # The key is a tuple of rmax, rmin
        key = str('({}, {})'.format(row[0], row[1]))

        results[key] = InlineDict({
            'SIGMAA'    : row[2],
            'SHIFT'     : row[3],
            'NEW_SIGMA' : row[4],
            'MEAN_W'    : row[5],
        })

    return results
# }}}

# Obtain all the other maps from the protein folder
def get_other_maps(path_to_protein):

    # List all directories in the protein path
    ls = os.listdir(path_to_protein)

    # Lambda to clean some data
    fmap = lambda f: True if f not in refs else False

    return [os.path.join(path_to_protein, f) for f in ls if fmap(f)]

# Dump all the data from a protein folder into a log file {{{
def JSON_pipeline(path_to_out_data, path_to_out_log):

    # Generate the paths to the different maps
    path_to_refmtz   = os.path.join(path_to_out_data, 'refmtz')
    path_to_refmap   = os.path.join(path_to_out_data, 'refmap')
    path_to_noisy    = os.path.join(path_to_out_data, 'noisy')
    path_to_denoised = os.path.join(path_to_out_data, 'denoised')

    # Some needed assertions before continuing
    assert os.path.exists(path_to_refmtz)
    assert os.path.exists(path_to_refmap)
    assert os.path.exists(path_to_noisy)
    assert os.path.exists(path_to_denoised)

    # Get the current protein
    protein = os.path.basename(os.path.dirname(path_to_out_data))

    # Get the current simulation name
    sim_name = os.path.basename(path_to_out_data)

    # Match some information in the simulation name
    matches = regex.match(SIMULATION_FORMAT, sim_name)

    # Parse the data from the reference builds
    refmtz   = parse_refmtz(os.path.join(path_to_refmtz, 'log'))
    refmap   = parse_map(os.path.join(path_to_refmap, 'log'))
    noisy    = parse_map(os.path.join(path_to_noisy, 'log'))
    denoised = parse_map(os.path.join(path_to_denoised, 'log'))

    # Dictionary containing the JSON output
    json_out = OrderedDict()
    
    # Add the data to the dictionary in order
    json_out['protein']      = protein
    json_out['space_group']  = refmtz['spgroup']
    json_out['resolution']   = refmtz['resrange']
    json_out['noise_std']    = float(matches.group(1))
    json_out['h_denoise']    = float(matches.group(2))
    json_out['r_comparison'] = float(matches.group(3))
    json_out['h_proportion'] = float(matches.group(4))
    json_out['pref_eps']     = float(matches.group(5))

    # Add a dictionary where the builds will be stored
    json_out['builds'] = OrderedDict()

    # Dump some results into the builds
    json_out['builds']['refmtz']   = dump_refmtz(refmtz)
    json_out['builds']['refmap']   = dump_map(refmap)
    json_out['builds']['noisy']    = dump_map(noisy)
    json_out['builds']['denoised'] = dump_map(noisy)

    # Path to the output file
    path_to_out_json = os.path.join(path_to_out_log, '{}_log.json'.format(protein))

    # Dump the JSON file into a dictionary
    with open(path_to_out_json, 'w') as f:

        # Generate the JSON encoded dictionary
        b = json.dumps(json_out, cls = CustomEncoder, indent = 4)

        # Replace the marks to make the list inline
        b = b.replace('"##<', '').replace('>##"', '').replace('\'', '"')

        # Write the data into the file stream
        f.write(b)

    return json_out
# }}}

if __name__ == '__main__':
    pass
