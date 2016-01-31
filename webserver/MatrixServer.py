#!/usr/bin/env python3

from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse
from os import kill, system
import signal
import subprocess

PORT = 80
HOSTNAME = ''
STATIC_CONTENT = 'static.html'
STATIC_FOOTER = 'staticFooter.html'
ENCODING = 'utf-8'

# no root no fun.
ANIMATION_BINARY_PATH = '/root/build/apps/'
ANIMATION_DICT = {
    1: 'matrix-clock',
    2: 'matrix-analogclock',
    3: 'matrix-chess',
    4: 'matrix-fullblink',
    5: 'matrix-cairoexample',
    6: 'matrix-circle',
    7: 'matrix-fire',
    8: 'matrix-zwickl',
    9: 'matrix-rain'
}

cur_anim_pid = -1

def animation_handler(no):
    global cur_anim_pid

    if no >= 100:
        if no == 100:
            system("poweroff")
        elif no == 101:
            system("reboot")
        elif no == 102:
            if cur_anim_pid != -1:
                kill(cur_anim_pid, signal.SIGKILL)
            cur_anim_pid = -1
        else:
            return 'Go home kiddy!'

        return 'Aye, aye!'

    if not no in ANIMATION_DICT:
        return 'Go home kiddy!'
    
    if cur_anim_pid != -1:
        kill(cur_anim_pid, signal.SIGKILL)
    
    proc = subprocess.Popen([ANIMATION_BINARY_PATH + ANIMATION_DICT.get(no), '-s', 'localhost'])
    cur_anim_pid = proc.pid
    return 'Aye, aye, captain!'
        

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        
        url = urlparse(self.path)
        params = parse_qs(url.query)

        status = ''
        if 'foo' in params:
            foo = params.get('foo')[0]
            if (foo.isdigit()):
                status = animation_handler(int(foo))

        self.wfile.write(static_content)
        self.wfile.write(bytes(status, ENCODING))
        self.wfile.write(static_footer)


myServer = HTTPServer((HOSTNAME, PORT), MyServer)

with open(STATIC_CONTENT, 'r') as f:
    static_content=bytes(f.read(), ENCODING)
with open(STATIC_FOOTER, 'r') as f:
    static_footer=bytes(f.read(), ENCODING)

animation_handler(1)
print('phasers ready to fire')

try:
    myServer.serve_forever()
except KeyboardInterrupt:
    pass

myServer.server_close()
