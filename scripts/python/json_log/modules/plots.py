import numpy as np
import matplotlib.pyplot as plt
import matplotlib as ml
import os

# User defined modules
from .parse import NestedJSON

__all__ = ['plot_parameters', 'plot_residues']

# Some global definitions
COLORS = ['#49443d', '#c1a188', '#0073e5', '#7ddc1f']
MARKER = ['+', 'x', '*', 'd']

def check_font_exists(font_name):
    ''' Check if a font exists in matplotlib. '''
    import matplotlib.font_manager as fm

    # Get all fonts available
    fonts = fm.findSystemFonts(fontpaths=None, fontext='ttf')

    # Loop over the fonts and find any ocurrence
    for font in fonts:
        if font_name in font:
            return True

    return False

# -- Set some matplotlib's parameters
ml.rcParams['mathtext.fontset'] = 'cm'
ml.rcParams['font.size'] = 12

if check_font_exists('iosevka-slab'): 
    ml.rcParams['font.family'] = 'Iosevka Slab'

# Plot the parameters from the log path for each build {{{
def plot_parameters(log_path, show = False):
    ''' Plot the parameters from a path where log is located. '''

    # Assert the existence of the log path
    assert os.path.exists(log_path)

    # List the log file from the path
    log_file = [f for f in os.listdir(log_path) if f.endswith('json')][0]

    # Generate a NestedJSON object to access the data easily
    json_dict = NestedJSON(os.path.join(log_path, log_file))

    # Get some important properties from the file
    protein    = json_dict['protein']
    spc_grp    = json_dict['space_group']
    resolution = json_dict['resolution']
    noise_std  = json_dict['noise_std']
    h_sqrt_den = json_dict['denoise_hsqrt']
    big_vol_e  = json_dict['big_vol_ext']
    small_vol_e = json_dict['small_vol_ext']

    # Generate a figure to plot the data into it
    fig = plt.figure(figsize = (10, 6))

    # Generate two axis for Rw and Rf
    axis_Rw = fig.add_subplot(1, 2, 1)
    axis_Rf = fig.add_subplot(1, 2, 2)

    # Set some important properties of the axis
    axis_Rw.set_xlabel('cycle'); axis_Rf.set_xlabel('cycle')
    axis_Rw.set_ylabel('Rw');    axis_Rf.set_ylabel('Rf')
    axis_Rw.grid('#ccc');        axis_Rf.grid('#ccc')
    
    # Set the title in the plot
    title = '{} ({} -- {}).  s = {}, h^2 = {}, b_e = {}, v_e = {}'
    fig.suptitle(title.format(
        protein, spc_grp, resolution, noise_std, 
        h_sqrt_den, big_vol_e, small_vol_e
        )
    )

    # Iterate through all possible builds in the log
    for b, (key, items) in enumerate(json_dict['builds'].items()):

        # Obtain the parameters of the build
        params = items['params']

        # Obtain the needed parameters Rw and Rf
        Rw_data, Rf_data = params['Rw'], params['Rf']

        # Generate the x-axis (1, len)
        cycles = np.arange(1, len(Rw_data['f']) + 1)

        # Plot the data into the axis
        axis_Rw.plot(
            cycles, Rw_data['f'], '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )
        axis_Rf.plot(
            cycles, Rf_data['f'], '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

    # Get the correct handles for the labels
    handles, labels = axis_Rw.get_legend_handles_labels()
    
    # Append the legend to the figure
    fig.legend(
        handles, labels, loc='upper center', 
        ncol = len(json_dict['builds'].keys()), frameon = False,
        bbox_to_anchor = (0, 0.95, 1, 0)
    )

    # Show the plot if wanted and save the data into a file
    if show: plt.show()
    plt.savefig(os.path.join(log_path, 'comp.pdf'))

# }}}

# Plot the residues from the log file {{{
def plot_residues(log_path, show = False):
    ''' Plot the parameters from a path where log is located. '''

    # Assert the existence of the log path
    assert os.path.exists(log_path)

    # List the log file from the path
    log_file = [f for f in os.listdir(log_path) if f.endswith('json')][0]

    # Generate a NestedJSON object to access the data easily
    json_dict = NestedJSON(os.path.join(log_path, log_file))

    # Get some important properties from the file
    protein    = json_dict['protein']
    spc_grp    = json_dict['space_group']
    resolution = json_dict['resolution']
    noise_std  = json_dict['noise_std']
    h_sqrt_den = json_dict['denoise_hsqrt']
    big_vol_e  = json_dict['big_vol_ext']
    small_vol_e = json_dict['small_vol_ext']

    # Generate a figure to hold the data
    fig = plt.figure(figsize = (14, 10))

    # Add several axis to contain the plots
    axis_residues   = fig.add_subplot(2, 3, 1)
    axis_prunning   = fig.add_subplot(2, 3, 2)
    axis_chains     = fig.add_subplot(2, 3, 3)
    axis_comp_build = fig.add_subplot(2, 2, 3)
    axis_comp_chain = fig.add_subplot(2, 2, 4)

    # Set some properties for each axis in the plot
    axis_residues.set_xlabel(r'cycle')
    axis_prunning.set_xlabel(r'cycle')
    axis_chains.set_xlabel(r'cycle')
    axis_comp_build.set_xlabel(r'cycle')
    axis_comp_chain.set_xlabel(r'cycle')

    axis_residues.set_ylabel(r'Residues')
    axis_prunning.set_ylabel(r'Residues after prunning') 
    axis_chains.set_ylabel(r'Residues uniquely allocated to chains')
    axis_comp_build.set_ylabel(r'Completeness by residues built')
    axis_comp_chain.set_ylabel(r'Completeness of chains')

    axis_residues.grid(color = '#ccc')
    axis_prunning.grid(color = '#ccc')
    axis_chains.grid(color = '#ccc')
    axis_comp_build.grid(color = '#ccc')
    axis_comp_chain.grid(color = '#ccc')

    # Iterate through all builds in the log file
    for b, (key, items) in enumerate(json_dict['builds'].items()):

        # Obtain the residues of the build
        residues = items['residues']

        # Get the needed parameters inside residues
        resb       = residues['resb']
        prunning   = residues['prunning']
        chains     = residues['chains']
        comp_build = residues['comp_build']
        comp_chain = residues['comp_chain']

        # Generate the x-axis (1, len)
        cycles = np.arange(1, len(resb) + 1)

        # Plot the residues per iteration in the file
        axis_residues.plot(
            cycles, [r[0] for r in resb], '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

        # Plot the prunning per cycle
        axis_prunning.plot(
            cycles, prunning, '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

        # Plot the chains per cycle
        axis_chains.plot(
            cycles, [c[0] for c in chains], '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

        # Plot the completeness by residues per cycle
        axis_comp_build.plot(
            cycles, comp_build, '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

        # Plot the completeness of chains per cycle
        axis_comp_chain.plot(
            cycles, comp_chain, '-', label = key, 
            color = COLORS[b], marker = MARKER[b]
        )

    # Get the correct handles for the labels
    handles, labels = axis_residues.get_legend_handles_labels()
    
    # Append the legend to the figure
    fig.legend(
        handles, labels, loc='upper center', 
        ncol = len(json_dict['builds'].keys()), frameon = False,
        bbox_to_anchor = (0, 0.95, 1, 0)
    )

    # -- Adjust the distance between subplots
    plt.subplots_adjust(
        left = 0.1, right = 0.9,
        wspace = 0.25, hspace = 0.2,
    )

    # Show the plot if wanted and save the data into a file
    if show: plt.show()
    plt.savefig(os.path.join(log_path, 'residues.pdf'))

# }}}

if __name__ == '__main__':
    pass
