import numpy as np
import os
import train
import struct
import pickle

uint8=True
perf_samples_file = 'samples.pickle'
perf_samples_dir = 'perf_samples'
if uint8:
    perf_samples_dir = 'perf_samples_uint8'
    perf_samples_file = 'samples_uint8.pickle'
    
cifar_10_dir = 'path to cifar-10-batches-py'

if __name__ == '__main__':
    if not os.path.exists(perf_samples_dir):
        os.makedirs(perf_samples_dir)

    label_output_file = open('y_labels.csv', 'w')

    _idxs = np.load('perf_samples_idxs.npy')
    train_data, train_filenames, train_labels, test_data, test_filenames, test_labels, label_names = \
        train.load_cifar_10_data(cifar_10_dir)
    
    # print(np.array(test_data[0]).shape)
    sample_data = {}

    for i in _idxs:
        _output_str = '{name},{classes},{label}\n'.format(name=test_filenames[i].decode('UTF-8')[:-3] + 'bin', classes=10, label=np.argmax(test_labels[i]))
        label_output_file.write(_output_str)
        if uint8:
            sample_img = np.round(np.array(test_data[i])/2).astype(np.int) # scale to (0,127) for int8 inference
        else:
            # int8
            sample_img = np.array(test_data[i])
        
        sample_img = sample_img.transpose((2,0,1)).flatten()  # transpose to (3,32,32)
        # sample_img = np.array(test_data[i]).flatten()

        f = open(perf_samples_dir + '/' + test_filenames[i].decode('UTF-8')[:-3] + 'bin', "wb")
        mydata = sample_img
        myfmt = 'B' * len(mydata)
        bin = struct.pack(myfmt, *mydata)
        f.write(bin)
        f.close()
        sample_data[test_filenames[i].decode('UTF-8')[:-4]] = sample_img
        
    with open(perf_samples_file, "wb") as f:
        pickle.dump(sample_data, f, pickle.HIGHEST_PROTOCOL)
        

    label_output_file.close()
