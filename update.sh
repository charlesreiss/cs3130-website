#eval "$(~/bin/rbenv init -)"
umask 022
#python3 quizzes_to_data.py    # XXX
#python3 recordings_to_data.py # XXX
chmod -R a+r files
chmod -R a+r slides
pd=/u/cr4bd/bin/pandoc

if [ ! -e jekyll-dir ]; then
    mkdir -p jekyll-dir
    pushd jekyll-dir
    ln -s ../Gemfile .
    ~/bin/bundle config set --local path vendor/bundle
    ~/bin/bundle install
    popd
fi

for input in assets Gemfile *.md _* files slides; do
    if [ ! -e jekyll-dir/$input ]; then
        ln -s ../$input jekyll-dir
    fi
done

for input in *.pmd */*.pmd; do
    output="jekyll-dir/${input%.pmd}.html"
    mkdir -p $(dirname $output)
#    if [ "$input" -nt "$output" ]; then 
        python3 env.py "$input" | \
        $pd --standalone \
            --to=html5 \
            --from=markdown+inline_code_attributes+smart+yaml_metadata_block \
            --number-sections \
            --table-of-contents --toc-depth=3 \
            --html-q-tags \
            --template=_template.html5 \
            -o "$output"
        chmod a+r $output
#    fi
done
pushd jekyll-dir
~/bin/bundle exec jekyll b --trace
