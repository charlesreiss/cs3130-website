import time, datetime, os

def set_oldest(dest, *src):
    d = 0
    for s in src:
        d2 = os.lstat(s).st_mtime
        if d2 > d: d = d2
    if d == 0:
        d = datetime.datetime.now().timestamp()
    os.utime(dest, (d,d))

if __name__ == '__main__':
    set_oldest('markdown/schedule.html', 'newcal.yaml', 'links.yaml')
