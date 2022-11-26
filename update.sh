#eval "$(~/bin/rbenv init -)"
umask 022
#python3 quizzes_to_data.py    # XXX
#python3 recordings_to_data.py # XXX
chmod -R a+r files
#chmod -R a+r slides
pd=/u/cr4bd/bin/pandoc
for input in *.pmd */*.pmd; do
    output="${input%.pmd}.html"
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
~/bin/bundle exec jekyll b --trace
