import numpy as np
import pandas as pd

"""
I didnt like having to copy things so instead i spent two hours writing this to extract the info from the peaks-process into the right format
"""
convert_peaks2D = False
convert_declarations_SiRi = False
convert_declarations_OSCAR = True
convert_time_OSCAR = False

if convert_peaks2D:
    # filename = "106Pd_recal"
    filename = "26thcal"
    name, peak2d_x,	peak2d_y, proj_x, proj_y = np.loadtxt(f"{filename}.csv",dtype = str, delimiter= '\t', unpack=True)

    def txt_remover(input_arr, b_or_f):
        txt_arr = np.array([f"{b_or_f}(proj)={i}" for i in range(8)])
        txt_rem = []
        for i in range(txt_arr.size):
            txt_rem.append(np.where(input_arr == txt_arr[i]))
        txt_rem_idx = np.sort(np.concatenate(np.concatenate(txt_rem)))

        temp_arr = np.float64(np.delete(input_arr, txt_rem_idx))
        
        output = []
        if b_or_f == "f":
            segment = [0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7] * 8
        elif b_or_f == "b":
            segment = np.concatenate([[i]* 16 for i in range(0,8)])
        
        for i in range(0, temp_arr.size , 2): #stepping with two for every i 
            output.append(f"{b_or_f}={segment[i]}")
            output.append(str(temp_arr[i]).replace('.', ','))
            output.append(str(temp_arr[i+1]).replace('.', ','))
            output.append(None)

        return output

    proj_x_rem = txt_remover(proj_x, "b")
    proj_y_rem = txt_remover(proj_y, "f")


    df = pd.DataFrame({"proj_x":proj_x_rem,
                    "proj_y":proj_y_rem})

    #NOTE: it is almost the right format! not quite though as it has dots instead of commas. excel doesnt like that maybe add b=x f=x if you feel nice

    df.to_excel(f"peaks2D_formatted_result_{filename}.xlsx", index = False)


if convert_declarations_SiRi:    
    """
    Quick script to convert into 'declarations.h' format 

    """
    # x, y = np.loadtxt("cal_values_E_SiRi.csv", delimiter = ',', unpack=True)
    # shift_pd= pd.read_excel("calibration_106Pd_16MeV_calibration2.xlsx", sheet_name = "SiRi shift and gain", )
    start = 17
    end = 80

    cal_excel = pd.read_excel("calibration_106Pd_16MeV_calibration2.xlsx", sheet_name = "SiRi cal 26th")
    shift_gain_pd_na = pd.DataFrame({"E shift": cal_excel["Unnamed: 21"][start:end],
                             "E gain": cal_excel["Unnamed: 22"][start:end],
                             "dE shift": cal_excel["Unnamed: 24"][start:end],
                             "dE gain": cal_excel["Unnamed: 25"][start:end]})

    shift_gain_pd = shift_gain_pd_na.fillna(1)

    print(shift_gain_pd)
    print("---------------- E shift E gain -------------------")
    for i in range(start,end):
        print("{" + f"{shift_gain_pd["E shift"][i]}, {shift_gain_pd["E gain"][i]}"+ "},")
        
    print()

    print("---------------- dE shift dE gain -------------------")
    for i in range(start,end):
        print("{" + f"{shift_gain_pd["dE shift"][i]}, {shift_gain_pd["dE gain"][i]}"+ "},")

    print()

if convert_declarations_OSCAR:
    start = 10
    end = 40

    oscar_cal_excel = pd.read_excel("calibration_106Pd_16MeV_calibration2.xlsx", sheet_name= "LaBr3 cal (3)")
    # print(oscar_cal_excel)
    shift_gain_pd = pd.DataFrame({"Gain": oscar_cal_excel["Unnamed: 1"][start:end],
                                     "Shift":oscar_cal_excel["Unnamed: 2"][start:end]})

    print("---------------- Shift and gain for OSCAR(LaBr3) -------------------")
    for i in range(start,end):
        print("{" + f"{shift_gain_pd["Shift"][i]}, {shift_gain_pd["Gain"][i]}"+ ", 0.}, // "+f"{i-9}")

# if convert_time_OSCAR
        