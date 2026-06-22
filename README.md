### Welcome to my FYS2810 project in calibrating 106Pd(p,p'gamma)106Pd data from the Oslo Cyclotron library!
In here you'll find everything but the actual data that was calibrated, so everything you need for the process but without the result. 
I'll outline what each file does for you so it'll be easy to navigate.

[FYS2810_project/FYS2810_project-3.pdf](FYS2810_project/FYS2810_project-3.pdf) contains the report for the project and should hopefully explain some choices. The .tex files to compile the report are all there as well. 

The [dataprocessing flow](FYS2810-flowchart-1.pdf) shows the flow of program usage.

The .root files which were produced are in this [OneDrive folder](https://uio-my.sharepoint.com/:f:/g/personal/hannawes_uio_no/IgCBuNUiYQPfQqnBOyT8jBaPAalezhv7SV36BsERl1wDRM0?e=3HAODO)

[read_data](read_data.cpp) when compiled with the correct declarations.h file it converts files from the native OCL .data-format to .root-files. Calibration coefficients are applied if they are in the declarations. Initially converted using plain coefficients. read_data will have to be rerun for each calibration(kind of silly i know).

[read_tree_27Al.cpp](read_tree_27Al.cpp) and its other friends read the .root data and make a .root file containing all the results like the h_LaBr3_energy plot and the excitation matrix. This is also where detector combinations are excluded if needed. Other things like the time gate and the [proton cut](27Al_proton_cut.root) is also set here, so that the excitation energies and coincidences are registered accordingly. 

[format-converter.py](format-converter.py) converts calibration coefficients made in (calibration_106Pd_16MeV_calibration2.xlsx)[calibration_106Pd_16MeV_calibration2.xlsx] and the [peaks2D.C](peaks2D.C) output to copy-pastable formats in the terminal. i did not like copying things bcs i often made errors. It has toggleables for what you want it to convert. You may need to provide the file drirectory and sheet of interest to use it. 

[resolution.ipynb](resolution.ipynb) takes centroid and sigma values from fits to calculate FWHM and resolution of said fits. 

[proj.C](proj.C) is a macro that plots projectionX of something. not needed tbh because it just does TH2D_object->ProjectionX("name","firstbin", "lastbin", "d"), but does helpfully plot separate detectors on top of each other.

[peaks2D.C](peaks2D.C) is a macro that lets you mark the ground states of particles(actually any state but the ground state is the most useful). It outputs it in a csv file that [format-converter.py](format-converter.py) can easily convert to a copy pastable .xlsx file. Do note that if the same filename is given to peaks2D.C, it will just continue writing on said file. 

[overlay_LaBr3_spectra.C](plotter_codes/overlay_LaBr3_spectra.C) overlays the wanted LaBr3 energy spectra files.

[plot_e_de_spectra.cpp](plotter_codes/plot_e_de_spectra.cpp) is a macro to plot overlaid E+ΔE spectra. The more useful [plot_ex_spectra.cpp](plotter_codes/plot_ex_spectra.cpp) plots the excitation energies on top of each other. Very nice. Can easily see the alignment this way. 

[/Qkins_calculations](Qkins_calculations) is a folder containing the ofset DeltaE and E energies in SiRi, as well as the excitation coefficients for 27Al and 106Pd.

[/figures](figures) contains some of the figures produced in this project.
