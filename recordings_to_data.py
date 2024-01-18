#!/usr/bin/python3

import datetime
import os
import os.path
import yaml
import re


def read_all_valid(dir):
    result = []
    for file in os.listdir(dir):
        if not file.endswith('-valid.empty'):
            continue
        result.append({
            'prefix': file.replace('-valid.empty', ''),
        })
    return result

if __name__ == '__main__':
    with open('./_data/recordings.yaml', 'w') as fh:
        yaml.dump(read_all_valid('/u/cr4bd/public_html/3130/S2024/recordings'), stream=fh)
