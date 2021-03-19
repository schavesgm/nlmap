import os
import regex
import json
import numpy

from .parse_utils import parse_refmtz
from .parse_utils import parse_map

__all__ = ['pipeline_JSON']

# Simulation name string format
SIM_FORMAT = 's(\d+.\d+)_h(\d+.\d+)_rs(\d+.\d+)_rc(\d+.\d+)'

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
def key_to_string(key):
    ''' Transform a given key to a string for readability. '''
    if key == 's':
        return 'noise_std'
    elif key == 'h':
        return 'denoise_param_square'
    elif key == 'b':
        return 'big_vol_extent'
    elif key == 'v':
        return 'small_vol_extent'

def dump_refmtz(refmtz_content):
    ''' Dump the contents of the reftmtz build. '''
    return {
        'params'   : dump_params(refmtz_content['params']),
        'residues' : dump_residues(refmtz_content['resids'])
    }

def dump_map(map_content):
    ''' Dump the contents of a map build. '''

    return {
        'params'   : dump_params(map_content['params']),
        'residues' : dump_residues(map_content['resids']),
        'sigma'    : dump_sigma(map_content['sigma'])
    }
# }}}

# Functions to convert the content dictionaries to good format {{{
def param_split(key, result_obj):
    ''' 
    Generate a dictionary containing the initial and
    final parameters.
    '''
    return {
        'o': InlineList([getattr(p, key)[0] for p in result_obj]),
        'f': InlineList([getattr(p, key)[1] for p in result_obj]),
    }

def dump_residues(residues_dict):
    '''
        Generate a dictionary containing the organised version
        of the residues data.
    '''
    return {
        'resb'       : InlineList([p.resb for p in residues_dict]),
        'prunning'   : InlineList([p.prun[0] for p in residues_dict]),
        'chains'     : InlineList([p.chain for p in residues_dict]),
        'comp_build' : InlineList([p.cbuild[0] for p in residues_dict]),
        'comp_chain' : InlineList([p.cchain[0] for p in residues_dict]),
    }

def dump_params(params_dict):
    '''
        Generate a dictionary containing the parameter results.
    '''
    return {
        'Rw'    : param_split('Rw', params_dict),
        'Rf'    : param_split('Rw', params_dict),
        'rms_L' : param_split('L', params_dict),
        'rms_A' : param_split('A', params_dict),
        'rms_V' : param_split('A', params_dict),
    }

def dump_sigma(sigma_array):
    '''
        Generate a dictionary containing the sigma results.
    '''

    # Dictionary containing the final results
    results = {}
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
def pipeline_JSON(path_out_protein, simulation_name, out_dir = './'):
    
    # Create out dir if it is not created
    os.makedirs(out_dir) if not os.path.exists(out_dir) else False

    # Generate some needed variables
    refmtz_path = os.path.join(path_out_protein, 'refmtz')
    refmap_path = os.path.join(path_out_protein, 'refmap')
    promap_path = os.path.join(path_out_protein, simulation_name)

    # Some needed assertions
    assert os.path.exists(refmtz_path)
    assert os.path.exists(refmap_path)
    assert os.path.exists(promap_path)

    # Obtain all the other directories in the folder
    other_maps = os.listdir(os.path.join(path_out_protein, simulation_name))

    # Save the protein name after assertions
    protein = os.path.basename(path_out_protein)

    # Match some information in the simulation name
    matches = regex.match(SIM_FORMAT, simulation_name)

    # Parse the data from the reference builds
    refmtz = parse_refmtz(os.path.join(refmtz_path, 'log'))
    refmap = parse_map(os.path.join(refmap_path, 'log'))

    # Dictionary containing the JSON output
    json_out = {
        'protein'       : protein,
        'space_group'   : refmtz['spgroup'],
        'resolution'    : refmtz['resrange'],
        'noise_std'     : float(matches.group(1)),
        'denoise_hsqrt' : float(matches.group(2)),
        'r_search'      : float(matches.group(3)),
        'r_comparison'  : float(matches.group(4)),
        'builds' : {
            'refmtz' : dump_refmtz(refmtz),
            'refmap' : dump_map(refmap),
        }
    }

    # Add the other maps to the content reference
    for processed_map in other_maps:

        # Generate the key for the given map
        key = os.path.basename(processed_map)

        # Path to processed map
        map_path = os.path.join(promap_path, processed_map)

        # Parse the data from the given map
        content = parse_map(os.path.join(map_path, 'log'))

        # Dump the content in the correct format
        json_out['builds'][key] = dump_map(content)

    # Path to the output file
    OUT_PATH = os.path.join(out_dir, '{}_log.json'.format(protein))

    # Dump the JSON file into a dictionary
    with open(OUT_PATH, 'w') as f:

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
