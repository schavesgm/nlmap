import gemmi, os, numpy

class Histomap:
    ''' Load a map and create a histogram from it if needed. '''

    def __init__(self, map_path):

        # Assert the existence of the map path
        assert os.path.exists(map_path)

        # Save some data into memory
        self.name = map_path
        self.map  = gemmi.read_ccp4_map(map_path)

        # Extend to the whole unit cell
        self.map.setup()

    @property
    def as_array(self):
        return numpy.array(self.map.grid, copy = False).flatten()

    def histogram(self, axis, n_bins, **kwargs):
        # Histogram the data inside axis
        axis.hist(self.as_array, n_bins, **kwargs)

if __name__ == '__main__':
    pass
