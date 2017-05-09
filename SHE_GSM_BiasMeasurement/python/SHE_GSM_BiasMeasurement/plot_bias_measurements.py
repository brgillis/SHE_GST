""" @file measurement_extraction.py

    Created 26 Apr 2017

    Main function to plot bias measurements.

    ---------------------------------------------------------------------

    Copyright (C) 2017 Bryan R. Gillis

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

import argparse
from os.path import join
from astropy.table import Table
import numpy as np
from scipy.interpolate import InterpolatedUnivariateSpline as Spline

import matplotlib
import matplotlib.pyplot as pyplot
matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True
matplotlib.rcParams['text.usetex'] = True

testing_data_keys = ("p", "s", "e")
testing_data_labels = {"p": "PSF Size",
                       "s": "Sky Level",
                       "e": "P(e)"}

testing_variant_labels = ("m2", "m1", "p0", "p1", "p2")
measurement_keys = ("mDIM", "mDIM_err", "cDIM", "cDIM_err")

x_values = {"p": [0.8,0.9,1.0,1.1,1.2],
            "s": [8.0608794667689825, 9.0670343062718768, 10.073467500059127,
                           11.07982970368346,  12.086264012414167],
            "e": [0.30241731263684996, 0.27099491059570091, 0.2422044791810781,
                  0.21333834512347458, 0.18556590758327751],
            }

x_ranges = {"p":(0.75,1.25),
            "s":(7.5,12.5),
            "e":(0.170,0.315)}

err_factor = np.sqrt(0.0001)
    
m_target = 1e-4
c_target = 5e-4

method_colors = {"KSB": "k",
                 "REGAUSS": "r",
                 "LensMC": "b",}
method_offsets = {"KSB": 0,
                  "REGAUSS": 0.01,
                  "LensMC": -0.01,}

fontsize = 12

def get_bias_measurement_filenames(file_name_root,key):
    
    base_tail = "_pp0_sp0_ep0.fits"
    
    filenames = []
    
    for testing_variant_label in testing_variant_labels:
        filenames.append(file_name_root + base_tail.replace(key+"p0",key+testing_variant_label))
    
    return filenames

def main():
    """ @TODO main docstring
    """
    
    # Set up the command-line arguments
    parser = argparse.ArgumentParser()
    
    parser.add_argument('--methods', nargs='*', default=["KSB","REGAUSS","LensMC"],
                        help='Methods to plot bias measurements for.')
    parser.add_argument('--data_folder', default="/home/brg/Data/SHE_SIM/sensitivity_testing/bias_measurements")
    parser.add_argument('--output_folder', default="/home/brg/Data/SHE_SIM/sensitivity_testing/plots")
    parser.add_argument('--output_file_name_root', default="sensitivity_testing")
    parser.add_argument('--output_format', default="png")
    
    args = parser.parse_args()
    
    # Read in data for each method
    bias_measurements_dict = {}
    for method in args.methods:
        bias_measurements = {}
        
        # Put together the filename root for the method
        method_file_name_root = join(args.data_folder,"bias_measurements_"+method)
        
        # Get data for each test we're running
        for testing_data_key in testing_data_keys:
            sens_testing_data = {}
            
            sens_testing_data["x"] = x_values[testing_data_key]
            
            for measurement_key_template in measurement_keys:
                for dim in range(3):
                    sens_testing_data[measurement_key_template.replace("DIM",str(dim))] = []
            
            bias_measurement_filenames = get_bias_measurement_filenames(method_file_name_root,testing_data_key)
            
            # Read the measurements from each file
            for bias_measurement_filename in bias_measurement_filenames:
                bias_measurement_table = Table.read(bias_measurement_filename)
                for row_i in range(3):
                    dim = bias_measurement_table["dimension"][row_i]
                
                    # Store the measured values
                    for measurement_key_template in measurement_keys:
                        sens_testing_data[measurement_key_template.replace("DIM",str(dim))].append(
                            bias_measurement_table[measurement_key_template.replace("DIM","")][row_i])
                
            
            # Store this in the measurements object for this method
            bias_measurements[testing_data_key] = sens_testing_data
        
        # Store this method's measurements in the global measurements object
        bias_measurements_dict[method] = bias_measurements
        
    # Plot the biases and errors for each measurement
    for testing_data_key in testing_data_keys:
            
        xlim = x_ranges[testing_data_key]
    
        for measurement_key_template in measurement_keys:
            
            # Plot regularly for dim = 0
            measurement_key = measurement_key_template.replace("DIM","")
            measurement_key_1 = measurement_key_template.replace("DIM","1")
            measurement_key_2 = measurement_key_template.replace("DIM","2")
        
            # Set up the figure
            fig = pyplot.figure()
            fig.subplots_adjust(wspace=0, hspace=0, bottom=0.1, right=0.95, top=0.95, left=0.12)
            
            ax = fig.add_subplot(1,1,1)
            ax.set_xlabel(testing_data_labels[testing_data_key],fontsize=fontsize)
            ax.set_ylabel("$"+measurement_key.replace("_err",r"_{\rm err}")+"$",fontsize=fontsize)
            
            # Plot points for each method
            for method in args.methods:
                
                sens_testing_data = bias_measurements_dict[method][testing_data_key]
                
                x_vals = np.array(np.add(sens_testing_data["x"],
                                method_offsets[method]*np.abs(sens_testing_data["x"][3]-sens_testing_data["x"][1])))
                y1_vals = np.array(np.subtract(sens_testing_data[measurement_key_1],sens_testing_data[measurement_key_1][2]))
                y2_vals = np.array(np.subtract(sens_testing_data[measurement_key_2],sens_testing_data[measurement_key_2][2]))
                
                if testing_data_key=="e":
                    x_vals = np.flipud(x_vals)
                    y1_vals = np.flipud(y1_vals)
                    y2_vals = np.flipud(y2_vals)
                    
                y_vals = np.sqrt(y1_vals**2+y2_vals**2)
                
                # Plot the values (and optionally error bars)
                if "_err" not in measurement_key:
                    err_key = measurement_key + "0_err"
                    y_errs = np.multiply(err_factor,sens_testing_data[err_key])
                    if testing_data_key=="e":
                        y_errs = np.flipud(y_errs)
                    ax.errorbar(x_vals, y_vals, y_errs, color=method_colors[method], linestyle='None')
                else:
                    y1_vals *= err_factor
                    y2_vals *= err_factor
                    y_vals *= err_factor
                ax.plot(x_vals, y_vals, color=method_colors[method], marker='o', linestyle='None')
                    
                # Calculate and plot an interpolating spline
                y1_spline = Spline(x_vals,y1_vals)
                y2_spline = Spline(x_vals,y2_vals)
        
                x_spline_vals = np.linspace(x_vals[0],x_vals[-1],100)
                
                y1_spline_vals = y1_spline(x_spline_vals)
                y2_spline_vals = y2_spline(x_spline_vals)
                y_spline_vals = np.sqrt(y1_spline_vals**2+y2_spline_vals**2)
                ax.plot(x_spline_vals, y_spline_vals, color=method_colors[method], marker='None',
                        label=method)
                    
                # Plot the target line
                if("m" in measurement_key):
                    target = m_target
                else:
                    target = c_target
                
                ax.plot(xlim,[target,target],label=None,color="k",linestyle="dashed")
                ax.plot(xlim,[-target,-target],label=None,color="k",linestyle="dashed")
                ax.plot(xlim,[20*target,20*target],label=None,color="k",linestyle="dotted")
                ax.plot(xlim,[-20*target,-20*target],label=None,color="k",linestyle="dotted")
                ax.plot(xlim,[0,0],label=None,color="k",linestyle="solid")
                
            # Set the limits and scale
            ax.set_xlim(x_ranges[testing_data_key])
            
            # Show the legend
            ax.legend(loc="lower right")
                    
                    
            # Save and show it
            output_filename = join(args.output_folder,args.output_file_name_root + "_" + 
                                   testing_data_key + "_" + measurement_key + "." + args.output_format)
            pyplot.savefig(output_filename, format=args.output_format, bbox_inches="tight", pad_inches=0.05)
            fig.show()
            
            # Now plot dim 1 v dim 2
        
            # Set up the figure
            fig = pyplot.figure()
            fig.subplots_adjust(wspace=0, hspace=0, bottom=0.1, right=0.95, top=0.95, left=0.12)
            
            ax = fig.add_subplot(1,1,1)
            ax.set_xlabel("$"+measurement_key_1.replace("_err",r"_{\rm err}")+"$",fontsize=fontsize)
            ax.set_ylabel("$"+measurement_key_2.replace("_err",r"_{\rm err}")+"$",fontsize=fontsize)
            
            # Plot points for each method
            for method in args.methods:
                
                sens_testing_data = bias_measurements_dict[method][testing_data_key]
                
                x_vals = np.array(np.add(sens_testing_data["x"],
                                method_offsets[method]*np.abs(sens_testing_data["x"][3]-sens_testing_data["x"][1])))
                y1_vals = np.array(np.subtract(sens_testing_data[measurement_key_1],sens_testing_data[measurement_key_1][2]))
                y2_vals = np.array(np.subtract(sens_testing_data[measurement_key_2],sens_testing_data[measurement_key_2][2]))
                
                if testing_data_key=="e":
                    x_vals = np.flipud(x_vals)
                    y1_vals = np.flipud(y1_vals)
                    y2_vals = np.flipud(y2_vals)
                    
                y_vals = np.sqrt(y1_vals**2+y2_vals**2)
                
                # Plot the values (and optionally error bars)
                if "_err" not in measurement_key:
                    err_key_1 = measurement_key + "1_err"
                    err_key_2 = measurement_key + "2_err"
                    y1_errs = np.multiply(err_factor,sens_testing_data[err_key_1])
                    y2_errs = np.multiply(err_factor,sens_testing_data[err_key_2])
                    if testing_data_key=="e":
                        y_errs = np.flipud(y_errs)
                    ax.errorbar(y1_vals, y2_vals, y1_errs, y2_errs, color=method_colors[method], linestyle='None')
                else:
                    y1_vals *= err_factor
                    y2_vals *= err_factor
                ax.plot(y1_vals, y2_vals, color=method_colors[method], marker='o', linestyle='None')
                    
                # Calculate and plot an interpolating spline
                y1_spline = Spline(x_vals,y1_vals)
                y2_spline = Spline(x_vals,y2_vals)
        
                x_spline_vals = np.linspace(x_vals[0],x_vals[-1],100)
                
                y1_spline_vals = y1_spline(x_spline_vals)
                y2_spline_vals = y2_spline(x_spline_vals)
                ax.plot(y1_spline_vals, y2_spline_vals, color=method_colors[method], marker='None',
                        label=method)
                    
            # Plot the target line
            if("m" in measurement_key):
                target = m_target
            else:
                target = c_target
                
            theta_vals = np.linspace(0,2*np.pi,360)
            
            xlim = ax.get_xlim()
            ax.set_xlim(xlim)
            ylim = ax.get_ylim()
            ax.set_ylim(ylim)
            
            ax.plot(target*np.cos(theta_vals),target*np.sin(theta_vals),label=None,color="k",linestyle="dashed",)
            ax.plot(20*target*np.cos(theta_vals),20*target*np.sin(theta_vals),label=None,color="k",linestyle="dotted")
            ax.plot(xlim,[0,0],label=None,color="k",linestyle="solid")
            ax.plot([0,0],ylim,label=None,color="k",linestyle="solid")
            
            # Show the legend
            ax.legend(loc="lower right")
                    
                    
            # Save and show it
            output_filename = join(args.output_folder,args.output_file_name_root + "_" + 
                                   testing_data_key + "_" + measurement_key + "_2D." + args.output_format)
            pyplot.savefig(output_filename, format=args.output_format, bbox_inches="tight", pad_inches=0.05)
            fig.show()
            
    return

if __name__ == "__main__":
    main()