import os, regex

class Parser:
    ''' Parser to obtain content from files. '''

    def __init__(self, fpath):

        # Assert if the file exists
        assert os.path.exists(fpath), \
        '-- %s does not exist' %(fpath)

        # Get the contents of the log file in a list
        with open(fpath, 'r') as f: self.content = f.readlines()

        # Name of the file
        self.name = os.path.basename(fpath)

    def get_line(self, regexp):
        ''' Get a line matching a regex. '''
        for line in self.content:
            if regex.match(regexp, line):
                return line.strip()

    def get_all_sections(self, beg_mark, end_mark):
        ''' Get all the sections between two marks. '''

        # Initial position and list containing results
        pos, final_results = 0, []

        # Get all possible results for the given marks
        while True:

            # Obtain the next section data
            result, pos = self.get_section(beg_mark, end_mark, pos)

            # Control the result
            if result != -1:
                final_results.append(result)
            else:
                break

        return final_results

    def get_section(self, beg_match, end_match, pos):
        ''' Get the data inside two marks from a given position. '''

        # Control variables
        found_sec = []; found = False

        # Iterate through all the lines in the file
        for idl, line in enumerate(self.content[pos:], pos):
            # Find the begining of a section
            if beg_match in line:
                # Iterate inside the section until end_mark is found
                for ids, subline in enumerate(self.content[idl + 1:], idl + 1):
                    if subline != '\n' and end_match not in subline:
                        found_sec.append(subline.strip())
                    if end_match in subline:
                        found = True; pos = ids; break

            # End iteration if section was found
            if found: break

        if found_sec:
            return found_sec, pos
        else:
            return -1, -1

    # Representation methods {{{
    def __str__(self):
        return f'<Parser: {self.name}>'

    def __repr__(self):
        return f'<Parser: {self.name}>'
    # }}}

if __name__ == '__main__':
    pass
