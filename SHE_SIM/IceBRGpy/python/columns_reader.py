""" @file columns_reader.py

    Created 20 Jul 2015

    A class to assist with reading in colums from an ascii table.

    ---------------------------------------------------------------------

    Copyright (C) 2015 Bryan R. Gillis

    This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import numpy as np
import astropy.io.ascii as ascii

class columns_reader(object):
    """ A class to assist with reading in colums from an ascii table.
    """

    def __init__(self):
        self._keys_ = []
        self._col_names_ = []
        self._offsets_ = []
        self._factors_ = []
        self._indices_from_keys_ = {}
        self._keys_from_indices_ = {}
        
        self._filename_ = None
        self._cols_changed_ = True
        self._read_in_data_ = []
        
        return
        
    def add(self, key, col_name, offset=0., factor=1.):
        
        i = len(self._col_names_)
        
        self._indices_from_keys_[key] = i
        self._keys_from_indices_[i] = key
        
        self._keys_.append(key)
        self._col_names_.append(col_name)
        self._offsets_.append(offset)
        self._factors_.append(factor)
        
        self._cols_changed_ = True
        
        return
        
    def read(self, filename, sort_by=None):
        
        if((self._cols_changed_) or (self._filename_ != filename)):

            # Load in the table
            try:
                data_table = ascii.read(filename)
            except:
                print("ERROR: Table " + filename + " cannot be read.")
                return
            
            if sort_by is not None:
                data_table.sort(sort_by)
            
            self._read_in_data_ = []
            for col_i in xrange(self.num_cols()):
                self._read_in_data_.append((np.array(data_table[self._col_names_[col_i]])+self._offsets_[col_i])*self._factors_[col_i])
            
            self._cols_changed_ = False
            self._filename_ = filename
            
        return self._read_in_data_
    
    def num_cols(self):
        return len(self._col_names_)
    
    def keys(self):
        return self._keys_
    
    def col_names(self):
        return self._col_names_
    
    def data(self):
        return self._read_in_data_
    
    def indices(self):
        return self._indices_from_keys_.values()
    
    def indices_from_keys(self):
        return self._indices_from_keys_
    
    def keys_from_indices(self):
        return self._keys_from_indices_
    
    def index(self, key):
        return self._indices_from_keys_[key]
    
    def key(self, index):
        return self._keys_from_indices_[index]
        