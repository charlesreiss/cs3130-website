#!/usr/bin/python3

import argparse
import shlex
import logging
import shutil
import subprocess
import tempfile

from pathlib import Path

# FIXME:
    # look for talk.qmd
    # use build.py
    # slides-quarto/name.html [single-file]
    # slides-quarto/name.pdf
    # slides-quarto/all.zip

def _logged_call(*args: list[str | Path], cwd: Path | None=None):
    logging.info('running %s in %s', args, cwd)
    subprocess.check_call(args, cwd=cwd)

def _quarto(args: argparse.Namespace, command: list[str | Path]):
    _logged_call(
        'quarto', *command,
        cwd=args.quarto_base
    )

def _build_py(args: argparse.Namespace, command: list[str | Path]):
    command_str =  ' '.join(map(shlex.quote, map(str, command)))
    _logged_call(
        'bash', '-c',
        f'source .venv/bin/activate && python3 build.py {command_str}',
        cwd=args.quarto_base
    )


def _decktape(args: argparse.Namespace, command: list[str]):
    command_str =  ' '.join(map(shlex.quote, map(str, command)))
    _logged_call(
        'bash', '-c',
        f'export NVM_DIR="$HOME/.nvm"; source "$NVM_DIR/nvm.sh"; npx decktape {command_str}',
        cwd=args.quarto_base
    )

def render_one(args: argparse.Namespace, name: str, output_dir: Path):
    with tempfile.TemporaryDirectory() as td_raw:
        td = Path(td_raw)
        _build_py(args, ['--figures', f'{name}/texfig'])
        _quarto(args,
            ['render', '--profile', 'selfcontained', name, '--output-dir', td]
        )
        talk_html = (td / name / 'talk.html')
        output_talk_html = output_dir / f'{name}.html'
        output_talk_html.parent.mkdir(exist_ok=True)
        shutil.copyfile(talk_html, output_dir / f'{name}.html')
        talk_pdf =  (output_dir / f'{name}.pdf').absolute()
        _decktape(args, ['reveal', '--fragments', '-s', '1920x1080', talk_html, talk_pdf])

def render_all(args: argparse.Namespace, output_dir: Path):
    for potential in args.quarto_base.iterdir():
        if potential.name.startswith('.'):
            continue
        if potential.is_dir():
            if (potential / 'talk.qmd').exists():
                render_one(args, potential.name, output_dir)


def make_argparser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument('--quarto-base', type=Path, default=Path('../cs3130-slides/quarto'))
    subparsers = parser.add_subparsers(title='subcomands')
    render_one_parser = subparsers.add_parser('render-one')
    render_one_parser.set_defaults(command='render-one')
    render_one_parser.add_argument('name', type=str)
    render_one_parser.add_argument('output_dir', type=Path)
    render_all_parser = subparsers.add_parser('render-all')
    render_all_parser.set_defaults(command='render-all')
    render_all_parser.add_argument('output_dir', type=Path)
    render_copy_parser = subparsers.add_parser('render-copy')
    render_copy_parser.set_defaults(command='render-copy')
    render_copy_parser.add_argument('names', type=str, nargs='+')
    render_copy_parser.add_argument('--singlefile-directory',
                                    default=Path('slides-quarto-singlefile'),
                                    type=Path)
    render_copy_parser.add_argument('--slides-directory',
                                    default=Path('slides'),
                                    type=Path)
    return parser

def main():
    parser = make_argparser()
    args = parser.parse_args()
    if args.command == 'render-copy':
        for name in args.names:
            render_one(args, name, args.singlefile_directory)
        for name in args.names:
            for suffix in ('.pdf', '.html'):
                shutil.copyfile(
                    args.singlefile_directory / (name + suffix),
                    args.slides_directory / (name + suffix)
                )
    elif args.command == 'render-one':
        render_one(args, args.name, args.output_dir)
    elif args.command == 'render-all':
        render_all(args, args.output_dir)
    else:
        assert False, args

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    main()
