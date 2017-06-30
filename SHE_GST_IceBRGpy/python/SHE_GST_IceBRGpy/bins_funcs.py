""" @file bins_funcs.py

    Created 20 Jul 2015

    Various functions for handling bins.

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

def setup_lin_bins(bins_min,bins_max,num_bins):
    
    step = float(bins_max-bins_min)/num_bins
    
    # Get arrays of the mins, maxes, and mids for each bin
    mins = np.linspace(start=bins_min,
                              stop=bins_max,
                              num=num_bins,
                              endpoint=False)
    maxes = mins + step
    mids = (mins+maxes)/2.
    bins =  np.transpose(np.vstack((mins,maxes)))
    
    return bins, mids

def setup_log_bins(bins_min,bins_max,num_bins):
    
    # Get arrays of the mins, maxes, and mids for each bin
    loglimits = np.linspace(start=np.log(bins_min),
                              stop=np.log(bins_max),
                              num=num_bins+1,
                              endpoint=True)
    limits = np.exp(loglimits)
    
    mins = limits[:-1]
    maxes = limits[1:]
    mids = (mins+maxes)/2.
    bins =  np.transpose(np.vstack((mins,maxes)))
    
    return bins, mids
    
def get_bin_index(x, bins):
    ibest = np.argmax(x<bins[:,1])
    if(ibest>0):
        return ibest
    if((x>=bins[0,0]) and (x<bins[0,1])):
        return 0
    else:
        return -1