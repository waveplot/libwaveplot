from ctypes import *

class Info(Structure):
     _fields_ = [("duration_secs", c_uint32),
                 ("num_channels", c_uint8),
                 ("bit_depth", c_uint16),
                 ("bit_rate", c_uint32),
                 ("sample_rate", c_uint32),
                 ("file_format", c_char_p)]

class AudioSamples(Structure):
     _fields_ = [("samples", POINTER(POINTER(c_float))),
		 ("length", c_size_t)]

lib = cdll.LoadLibrary("libwaveplot.so.1.0")

lib.init()

lib.alloc_info.restype = POINTER(Info)
lib.alloc_audio_samples.restype = POINTER(AudioSamples)

i = lib.alloc_info()
f = lib.alloc_file()

print("Allocated!")

lib.load_file("./test.flac",f)
print("Loaded!")
lib.get_info(f,i)
print("Info'd!")
print(i.contents.duration_secs)
print(i.contents.num_channels)
print(i.contents.bit_depth)
print(i.contents.bit_rate)
print(i.contents.sample_rate)
print(i.contents.file_format)

a = lib.alloc_audio_samples()

sums = []
while lib.get_samples(f,i,a) != 0:
    this_sum = 0
    for x in range(a.contents.length):
        this_sum += a.contents.samples[0][x]*a.contents.samples[0][x]

    print("{:.0f}".format(this_sum))
    sums.append(this_sum)
	
