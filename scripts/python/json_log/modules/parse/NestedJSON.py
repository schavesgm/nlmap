import json

class NestedJSON:
    ''' Wrapper around dict to access nested dictionaries. '''

    def __init__(self, path):

        # Load the data into memory
        with open(path, 'r') as f:
            self.data = json.load(f)

    def __getitem__(self, item):

        # Split the item by dots
        split = item.split('.')

        result = self.data[split[0]]

        for key in split[1:]:
            result = result[key]

        return result

if __name__ == '__main__':
    pass
