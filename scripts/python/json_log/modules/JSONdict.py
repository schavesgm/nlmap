import json

class JSONdict:

    def __init__(self, path):
        ''' Load the data from the path. '''

        # Load the data from the path
        with open(path, 'r') as f:

            # Load the JSON data
            self.data = json.load(f)

    def __getitem__(self, key):
        ''' Get the item from a dictionary using one liners. '''

        # Split the key using the .
        split_keys = key.split('.')

        # Access the first layer of the dictionary
        item = self.data[split_keys[0]]

        for k in split_keys[1:]:
            item = item[k]

        return item

if __name__ == '__main__':
    pass
