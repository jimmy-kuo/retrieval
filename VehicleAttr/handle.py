

import os
import sys
import codecs
model_class = 'mcls.txt'
model_mapping = 'model_mapping_tabel.txt'
mode_output = "model_table.txt"
model_dict = {}
with open(model_mapping, 'r') as f:
    while True:
        res = f.readline()
        if not res:
            break
        sp = res.strip().split(' ')
        num = int(sp[0])
        target = int(sp[1])
        model_dict[num] = target

fo = open(mode_output, 'w')
with open(model_class, "r") as f:
    while True:
        res = f.readline()
        if not res:
            break
        sp = res.strip().split(' ')
        print int(sp[0])
        if not int(sp[0]) in model_dict.keys():
            print sp[0], " not in the model dict"
            continue
        fo.write("%d %s\n"% (int(model_dict[int(sp[0])]), sp[1]))

fo.close()