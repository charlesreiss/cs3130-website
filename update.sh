#eval "$(~/bin/rbenv init -)"
umask 022
#python3 quizzes_to_data.py    # XXX
#python3 recordings_to_data.py # XXX
chmod -R a+r files
#chmod -R a+r slides
~/bin/jekyll b
