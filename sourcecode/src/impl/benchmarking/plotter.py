import matplotlib.pyplot as plt
import numpy as np
import csv  

def readCSV(filename):
    dict_xpoints = {}
    dict_ypoints_marking_size = {}
    dict_ypoints_table_size = {}
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        line_count = 0
        for row in reader:
            if line_count == 0:
                # print(f'Column names are {", ".join(row)}')
                line_count += 1
            elif row == []:
                continue 
            else:
                # print(f'\t{row[0]} and {row[1]} and {row[2]} and {row[3]}.')
                if row[0] not in dict_xpoints:
                    dict_xpoints[row[0]] = []
                    dict_ypoints_marking_size[row[0]] = []
                    dict_ypoints_table_size[row[0]] = []
                dict_xpoints[row[0]].append(int(row[1]))
                dict_ypoints_marking_size[row[0]].append(int(row[3]))
                dict_ypoints_table_size[row[0]].append(int(row[2]))
                line_count += 1
        print(f'Processed {line_count} lines.')

    return [dict_xpoints, dict_ypoints_marking_size, dict_ypoints_table_size]



def plot(xpoints, ypoints, yname):
    fig, ax = plt.subplots()
    for key in xpoints:
        ax.plot(xpoints[key], ypoints[key], label=key)
    # plt.plot(xpoints, ypoints)
    # image_format = 'svg' # e.g .png, .svg, etc.
    # image_name = 'myimage.svg'
    # ax.legend(loc='upper center', 
    # bbox_to_anchor=(0.5, 1.35),)

    # plt.legend(bbox_to_anchor =(1.44,-0.10), loc='upper right')
    plt.legend(loc='upper right')
    # ncol=3, )
    plt.xlabel('iterations')
    plt.ylabel(yname)
    plt.show()



[x,y1,y2] = readCSV("./benchmarkResults/mist/iter_data/mist_iter.csv")

# [x,y1,y2] = readCSV("./benchmarkResults/wahl-kroening/iter_data_easy_all.csv")

plot(x,y2, 'table size')
# plt.plot([1,2,3,4,5], [1000,2000,3000,4000,5000])
# plt.plot([1,2,3], [10,20,1000])
# plt.show()