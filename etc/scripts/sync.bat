cd ..
cd ..

git checkout master

:: Let's push our changes back to the subtrees...
git subtree push --prefix=lib/LibDS git@github.com:wint-3794/libds.git master

:: Let's download the changes in the subtrees...
git subtree pull --prefix=lib/LibDS git@github.com:wint-3794/libds.git master
