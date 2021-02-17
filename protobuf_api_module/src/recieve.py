import os 
import select 
import model_pb2
import struct 
import google.protobuf.message as message
from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32




def get_message_size(fifo):
    msg_size_bytes = os.read(fifo, 4) 
    return struct.unpack("<I", msg_size_bytes)[0]



PIPE_NAME = "mypipe" 
os.mkfifo(PIPE_NAME) 
try:
    fifo = os.open(PIPE_NAME, os.O_RDONLY | os.O_NONBLOCK) 
    try:
        poll = select.poll() 
        poll.register(fifo, select.POLLIN) 
        try:
            while True: 
                if (fifo, select.POLLIN) in poll.poll(2000):
                    size = get_message_size(fifo)
                    print(f"Got header: {size} ")
                    content = os.read(fifo, size)
                    frame = model_pb2.Frame()
                    frame.ParseFromString(content) 
                    print("got content") 
                    print(frame.label)
                    print(frame.obj)
                    print(frame.rect_inf) 
                    print(frame.flt)
                    print(type(frame))
                    print(frame) 
                    break
                else:
                    print("Nothing yet") 
        finally:
            poll.unregister(fifo) 
    finally:
        os.close(fifo)
finally:
    os.remove(PIPE_NAME) 




