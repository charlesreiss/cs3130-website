#eval "$(~/bin/rbenv init -)"
    
umask 022
python3 quizzes_to_data.py    # XXX
python3 recordings_to_data.py # XXX
chmod -R a+r files
chmod -R a+r oldquiz
chmod -R a+r readings/files
chmod -R a+r slides
chmod -R a+rX ~/public_html/3130/F2024/recordings
pd=/u/cr4bd/bin/pandoc

if [ ! -e $pd ]; then
    pd=pandoc
fi

BUNDLE=bundle

if [ ! -e jekyll-dir ]; then
    mkdir -p jekyll-dir
    pushd jekyll-dir
    ln -s ../Gemfile .
    $BUNDLE config set --local path vendor/bundle
    $BUNDLE install
    popd
fi

for input in assets Gemfile _* files slides; do
    if [ ! -e jekyll-dir/$input ]; then
        ln -s ../$input jekyll-dir
    fi
done

find . \( -name _\* -prune -o -name .?\* -prune -o -name jekyll-dir -prune \
          -o -name \*.pmd -prune \) -o -type f -print0 | \
    while IFS= read -r -d '' input; do
        dest_dir=$(dirname jekyll-dir/$input)
        mkdir -p "$dest_dir"
        output="jekyll-dir/$input"
        if [ "$input" -nt "$output" ]; then 
            cp -p "$input" "jekyll-dir/$input"
        fi
    done

for input in *.pmd */*.pmd; do
    output="jekyll-dir/${input%.pmd}.html"
    mkdir -p $(dirname $output)
    if [ "$input" -nt "$output" ]; then 
        python3 env.py "$input" | \
        $pd --standalone \
            --to=html5 \
            --from=markdown+inline_code_attributes+smart+yaml_metadata_block \
            --number-sections \
            --table-of-contents --toc-depth=3 \
            --html-q-tags \
            --katex=/~cr4bd/katex \
            --template=_template.html5 \
            -o "$output"
        chmod a+r $output
    fi
done
pushd jekyll-dir
chmod -R a+r readings/*
chmod -R a+r labhw/*
chmod -R a+r labhw/files/*
$BUNDLE exec jekyll b --trace "${@}"
