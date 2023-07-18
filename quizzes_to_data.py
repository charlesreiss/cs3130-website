#!/usr/bin/python3

import datetime
import os
import os.path
import yaml
import re

def read_quiz(quiz_file):
    quiz_name, _ = os.path.splitext(os.path.basename(quiz_file))
    result = {}
    with open(quiz_file, 'r') as fh:
        for line in fh:
            if ":" not in line:
                break
            key, value = line.split(":", 1)
            key = key.strip()
            value = value.strip()
            if key == 'open' or key == 'due':
                value = datetime.datetime.strptime(value, '%Y-%m-%d %H:%M')
                result[key + '_date']  = value.strftime('%Y-%m-%d')
                result[key + '_time']  = value.strftime('%H:%M')
            result[key] = value
    result['title'] = re.sub(r'\s*\(.*\)', '', result['title'])
    result['name'] = quiz_name
    return result

def read_all_quizzes(quiz_dir):
    result = []
    for quiz_file in os.listdir(quiz_dir):
        if not quiz_file.endswith('.md'):
            continue
        if 'hw' in quiz_file or 'lab' in quiz_file or 'cut' in quiz_file or 'placeholder' in quiz_file:
            continue
        result.append(read_quiz(os.path.join(quiz_dir, quiz_file)))
    return result

if __name__ == '__main__':
    with open('./_data/quizzes.yaml', 'w') as fh:
        yaml.dump(read_all_quizzes('../cs3130/quizzes'), stream=fh, canonical=True)
