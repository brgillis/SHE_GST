import galsim
from cProfile import runctx

def main():

    # Using very low accuracy GSParams here for speed
    gsparams = galsim.GSParams(minimum_fft_size=256,
                               folding_threshold=0.1,
                               kvalue_accuracy=1e-3,
                               stepk_minimum_hlr=2.5,)
    
    # Note - we actually use an interpolated image instead; just putting this in
    # so you can run the code without needing that file
    psf_prof = galsim.OpticalPSF(lam=725, # nm
                                 diam=1.2, # m
                                 defocus=0,
                                 obscuration=0.33,
                                 nstruts=3,
                                 gsparams=gsparams)
    
    convolved_image = galsim.Image(256,256,scale=0.02)

    for _i in range(1000):

        gal_prof = galsim.Sersic(n=4,half_light_radius=0.3,gsparams=gsparams)
        convolved_prof = galsim.Convolve(gal_prof,psf_prof,gsparams=gsparams)
        
        # Using no pixel method here since we plan to use a PSF profile
        # which already includes the pixel response
        convolved_prof.drawImage(convolved_image,method='no_pixel')

if __name__ == "__main__":
    runctx("main()",{},{"main":main},filename="convolve_time_test.prof")