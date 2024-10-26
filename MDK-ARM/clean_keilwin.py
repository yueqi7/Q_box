import os

index = 0
start_line = []
stop_line = []
current_directory = os.path.dirname(__file__)
entries = os.listdir(current_directory)
for entry in entries:
    if ".uvguix." in  entry:
        file_fullname = os.path.join(current_directory, entry)
        file = open(file_fullname, "r")
        lines = file.readlines()
        for line in lines:
            # print(line)
            if "<Doc>" in line:
                start_line.append(index)
            if "</Doc>" in line:
                stop_line.append(index)
            index += 1
        if len(start_line) != len(stop_line):
            print("doc 标记个数不匹配")
            file.close()
            exit(-1)
        for i in range(0, len(start_line)):
            for j in range(start_line[i], stop_line[i]+1):
                print("del ", lines[j])
                lines[j]=""
        file.close()
        os.remove(file_fullname)
        file = open(file_fullname, "w")
        for line in lines:
            if line != "":
                file.write(line)  
        file.close()
