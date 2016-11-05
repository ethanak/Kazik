#!/usr/bin/env python
#coding: utf-8;

from PIL import Image
import cStringIO, subprocess, sys, os, signal

outname = sys.argv[1]



LCDWIDTH = 84
LCDHEIGHT = 48

buffer = []

def getPixel(x, y):
    global buffer, LCDWIDTH, LCDHEIGHT
    return (buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1

def feed(line):
    global buffer
    line = line.strip()
    while len(line) > 1:
        a = line[:2]
        line = line[2:]
        buffer.append(int(a,16))

rend, wend = os.pipe()
pid = os.fork()
if pid < 0:
    raise Exception("Fork nie zadziałał")
if pid == 0:
    os.close(wend)
    os.dup2(rend,0)
    os.execvp('avconv',
        ["avconv", "-f", "image2pipe", "-vcodec", "ppm",
        "-i", "-", "-r", "10", "-vcodec", "libx264",
        "-crf", "18", "-y", outname])
    exit(0)
os.close(rend)
if True:
    for a in open('serial.txt','r'):
        if 'SCREENSHOT' in a:
            buffer = []
            continue
        if not a.startswith('='):
            if buffer is not None:
                feed(a)
            continue
        if len(buffer) < 504:
            print len(buffer)
            continue
        im = Image.new('L',(2*84 + 10, 2 * 48 + 10), 255)

        for y in range(48):
            for x in range(84):
                im.putpixel((2*84 + 10 -(2*x+5),2 * 48 + 10 - (2*y+5)),255 if not getPixel(x,y) else 0)
                im.putpixel((2*84 + 10 -(2*x+5),2 * 48 + 10 - (2*y+6)),255 if not getPixel(x,y) else 0)
                im.putpixel((2*84 + 10 -(2*x+6),2 * 48 + 10 - (2*y+6)),255 if not getPixel(x,y) else 0)
                im.putpixel((2*84 + 10 -(2*x+6),2 * 48 + 10 - (2*y+5)),255 if not getPixel(x,y) else 0)
        buffer = None
        s = cStringIO.StringIO();
        im.save(s,'ppm')
        os.write(wend,s.getvalue())
        s.close()
#except KeyboardInterrupt:
#    os.kill(pid,signal.SIGINT)
#    exit(1)
#except:
#    os.kill(pid,signal.SIGINT)
#    raise
os.close(wend)
p,status=os.waitpid(pid,0)
if status:
    print "Avconv się wykrzaczył"
    exit(1)

