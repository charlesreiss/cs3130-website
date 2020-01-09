#!/bin/bash

here="$(dirname "$(readlink -m "$0")")/"
self="$(readlink -m "$0")"
cd "$here"
target="$HOME/public_html/COA2/S2020/"

if ls ~/.local/pandoc*/bin/pandoc
then pd="$(ls  ~/.local/pandoc*/bin/pandoc | tail -1)"
else pd="$(which pandoc)"
fi
which python3 || module load python3

if [ "$#" -lt 1 ] || [ "$1" != 'test' ]
then
    git commit -a -m 'autocommit caused by update'
    git pull
    git push
elif [ "$1" = 'test' ]
then
    shift
fi

if [ -n "$1" ]; then always=1; else always=; fi


# copies, with permission fix (644 for files, 755 for dirs), making dirs if needed
# also runs markdown, fixing links, if applicable
function upfile_inner() {
    src="$1"
    dst="$2"
    echo "$src → $dst"
    #return 1
    if [ ! -d "$(dirname "$dst")" ]
    then
        echo "making directory $(dirname "$dst")"
        mkdir -m 755 -p "$(dirname "$dst")"
    fi
    if [ "${src%.md}" != "$src" ]
    then
        tail="$(readlink -f "$src")"
        tail="${tail#${here}markdown/}"
        prefix="$(dirname "$(echo "$tail" | sed 's;[^/]*/;../;g')")"
        if [ "${#prefix}" -lt 2 ]; then prefix=''; else prefix=$prefix/; fi
        datetime="$(stat -c '%y' "$src" | cut -d':' -f-2)"

        python3 $here/env.py "$src" | \
        $pd --standalone \
            --to=html5 \
            --from=markdown+inline_code_attributes+smart \
            --number-sections \
            --title-prefix="DMT1" \
            --table-of-contents --toc-depth=3 \
            --css=${prefix}style.css \
            --katex='https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.11.1/' \
            --html-q-tags \
            --template=${here}template.html \
            --variable=year:${datetime:0:4} \
            --variable=datetime:"$datetime" \
            -o "$dst"
        sed -i \
            -e 's;</?colgroup>;;g' -e 's;<col [^>]*/>;;g' \
            -e 's/<span class="co">\&#39;/<span class="st">\&#39;/g' \
            -e 's/<span class="co">\&quot;/<span class="st">\&quot;/g' \
            -e 's/<table style="width:[^"]*">/<table>/g' \
            "$dst"
        sed -i 's/XXXX-XX-XX/'"$datetime"'/g' "$dst"
        sed -i "s;\(href=[\"']\)\.\?/;\1$prefix;g" "$dst"
        sed -i 's/<a href="'"$(basename "$dst")"'">\([^<]*\)<\/a>/<span class="visited">\1<\/span>/g' "$dst"
        chmod 664 "$dst"
        #touch --date="$(stat -c "%y" "$src")" "$dst"
    else
        cp "$src" "$dst" #  --preserve=timestamps
        chmod 644 "$dst"
    fi
}

# computes destination and checks if update needed
function upfile() {
    src=$(readlink -f "$1")
    tail=${src#${here}markdown/}
    if [ "$tail" = "$src" ];
    then
        echo "ERROR: Cannot update outside the markdown file tree" >&2
        echo "    error with" $src >&2
        echo "    here=$here" >&2
        return 1
    fi
    dst="${target}$tail"
    if [ "${src%.md}" != "$src" ]
    then
        dst="${dst%.md}.html"
    fi
    
    if [ -n "$always" ] \
    || [ "$src" -nt "$dst" ]; then upfile_inner "$src" "$dst"; return 0; fi
    
    if [ "${src%.md}" != "$src" ]; then
        if [ "$dst" -ot "${here}template.html" ] \
        || [ "$dst" -ot "${here}env.py" ] \
        || [ "$dst" -ot "$self" ]; then upfile_inner "$src" "$dst"; return 0; fi
    fi
}



# cross-link cal.yaml and the various files it makes
if [ cal.yaml -nt schedule.html ] \
|| [ links.yaml -nt schedule.html ] \
|| [ cal2html.py -nt schedule.html ] \
|| [ cal.yaml -nt markdown/cal.ics ] \
|| [ cal.yaml -nt assignments.json ]
then
    echo "doing schedule"
    python3 cal2html.py
    scp "assignments.json" "kytos.cs.virginia.edu:/var/www/html/coa2/meta/"
    scp "coursegrade.json" "kytos.cs.virginia.edu:/var/www/html/coa2/meta/"
fi
if [ schedule.html -nt markdown/schedule.md ]
then
    touch --date="$(stat -c "%y" schedule.html)" markdown/schedule.md
fi

# move all files to the destination tree, compiling as needed
find markdown -type f | while read path
do
    upfile "$path"
done



#rsync --update --compress --recursive --times --verbose -e ssh "$target" "$remote"

