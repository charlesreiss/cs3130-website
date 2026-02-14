#!/usr/bin/python3

from pathlib import Path

import datetime
import os
import os.path
import yaml
import re

def read_slides_from(dir: Path):
    result_dict = {}
    for item in dir.iterdir():
        if item.suffix == '.pdf' or item.suffix == '.html':
            if item.stem not in result_dict:
                result_dict[item.stem] = {}
            result_dict[item.stem][item.suffix[1:]] = item.name
    result = []
    for key in sorted(result_dict.keys()):
        current = {
            'name': key,
        }
        for suffix, filename in result_dict[key].items():
            current[suffix] = item.name
        if 'html' in current and 'pdf' in current:
            current['link'] = f'<a href="slides/{key}.html">{key}</a><sup>(<a href="slides/{key}.pdf">pdf</a>)</sup>'
        else:
            current['link'] = f'<a href="slides/{key}.pdf">{key}</a>'
        result.append(current)
    return result

if __name__ == '__main__':
    with open('./_data/slides.yaml', 'w') as fh:
        yaml.dump(read_slides_from(Path('slides')), stream=fh)
