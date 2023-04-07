#!/usr/bin/env python3
# This script is for converting inputs from the popular Game of Life "RLE" input format into
# the format our program expects.

import argparse
import re

def read_rle(fh):
    x = 0
    y = 0
    while True:
        line = fh.readline()
        if line.startswith('#'):
            continue
        if x == 0:
            m = re.match(r'x = (?P<x>\d+), y = (?P<y>\d+)', line)
            x = int(m.group('x'))
            y = int(m.group('y'))
            break
    remaining = fh.read()
    cur_count = 0
    cur_line = ''
    lines = []
    for c in remaining:
        if c in "0123456789":
            cur_count *= 10
            cur_count += int(c)
        elif c == 'b':
            cur_line += '.' * max(cur_count, 1)
            cur_count = 0
        elif c == 'o':
            cur_line += 'o' * max(cur_count, 1)
            cur_count = 0
        elif c == '$':
            cur_line += '.' * (x - len(cur_line))
            lines.append(cur_line)
            if cur_count > 1:
                for i in range(cur_count - 1):
                    lines.append('.' * x)
            cur_line = ''
            cur_count = 0
        elif c == '!':
            cur_line += '.' * (x - len(cur_line))
            lines.append(cur_line)
            break
    return lines 
                    

def make_input(
        original,
        output,
        left,
        right,
        above,
        below,
    ):
    with open(original, 'r') as fh:
        lines = read_rle(fh)
    blank_line = '.' * (len(lines[0]) + left + right)
    lines = list(map(lambda x: '.' * left + x + '.' * right, lines))
    lines = [blank_line] * above + lines + [blank_line] * below
    with open(output, 'w') as fh:
        fh.write('%s %s\n' % (len(lines[0]), len(lines)))
        for line in lines:
            fh.write(line + '\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Game of Life input in RLE format')
    parser.add_argument('output', help='output file in our format')
    parser.add_argument('--left', type=int, default=10, help='amount of blank cells to add to left')
    parser.add_argument('--right', type=int, default=10, help='amount of blank cells to add to right')
    parser.add_argument('--above', type=int, default=10, help='amount of blank cells to add above')
    parser.add_argument('--below', type=int, default=10, help='amount of blank cells to add below')
    class SetAllSidesAction(argparse.Action):
        def __call__(self, parser, namespace, value, option_string=''):
            namespace.above = namespace.below = namespace.left = namespace.right = value
    parser.add_argument('--expand', type=int, action=SetAllSidesAction,
        help='set left, right, above, below to same value')
    args = parser.parse_args()
    make_input(
        args.input,
        args.output,
        left=args.left,
        right=args.right,
        above=args.above,
        below=args.below
    )
