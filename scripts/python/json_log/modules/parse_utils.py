import os
import regex as re

__all__ = ['parse_refmtz']

# Obtain the Parser object to parse data from the files
from .Parser import Parser

# Obtain some functions to clean the extracted data
from .clean_parsed import clean_params
from .clean_parsed import clean_residues
from .clean_parsed import clean_sigma
from .clean_parsed import clean_resparams

# Globals for the module
BEG_PARAM = 'Final results'
END_PARAM = '<B><FONT COLOR="#FF0000"><!--SUMMARY_BEGIN-->'
BEG_RESID = '$TEXT:Result: $$ $$'
END_RESID = '$$'

def parse_refmtz(path):
    ''' Parse the content of the reference .mtz build. '''

    # Assert the existence of the path
    assert os.path.exists(path), '%s does not exist' %(path)

    # Generate a parser object
    cad = Parser(os.path.join(path, 'cad_input.log'))
    buc = Parser(os.path.join(path, 'buccaneer.log'))

    # Strings to define the cad parser matches
    sg_str = '.*Space group.*'
    rr_beg, rr_end = 'Resolution Range', 'Sort Order'

    # Parse the data from the cad file
    space_group = cad.get_line(sg_str)
    res_range   = cad.get_section(rr_beg, rr_end, 0)[0]

    # Parse the data from the buccaneer file
    params_results = buc.get_all_sections(BEG_PARAM, END_PARAM)
    resids_results = buc.get_all_sections(BEG_RESID, END_RESID)

    # Lists containing the cleaned results
    params, resids = clean_resparams(
        params_results, resids_results
    )

    # Get the space group from the data
    sp_group  = re.findall(r'\'.*\'', space_group)[0]

    # Dictionary containing the results
    results_reftmtz = {
        'spgroup' : sp_group.replace('\'', ''),
        'resrange': re.findall(r'\d+.\d+ A', res_range[0])[0],
        'params'  : params,
        'resids'  : resids
    }

    return results_reftmtz

def parse_map(path):
    ''' Parse a map file and extract all the relevant content from it. '''

    # Assert the existence of the path
    assert os.path.exists(path), '%s does not exist' %(path)

    # Generate two parse objects
    sig = Parser(os.path.join(path, 'sigma.log'))
    buc = Parser(os.path.join(path, 'buccaneer.log'))

    # Strings to define the sigma parser matches
    beg_sigma = 'Iteration Number'
    end_sigma = '<B><FONT COLOR="#FF0000"><!--SUMMARY_BEGIN-->'

    # Parse all the sigma sections from the sig parser
    sigma_results = sig.get_section(beg_sigma, end_sigma, 0)[0]

    # Parse the data from the buccaneer file
    params_results = buc.get_all_sections(BEG_PARAM, END_PARAM)
    resids_results = buc.get_all_sections(BEG_RESID, END_RESID)

    # Lists containing the cleaned results
    params, resids = clean_resparams(
        params_results, resids_results
    )

    # Clean the sigma values from the data
    cleaned_sigma = clean_sigma(sigma_results)

    # Dictionary containing the relevant information 
    results_map = {
        'name'    : os.path.basename(path),
        'params'  : params,
        'resids'  : resids,
        'sigma'   : cleaned_sigma
    }

    return results_map

if __name__ == '__main__':
    pass
