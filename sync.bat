git checkout master

:: Let's push our changes back to the subtrees...
git subtree push --prefix=lib/LibDS git@github.com:wint-3794/libds.git master
git subtree push --prefix=lib/Falcon git@github.com:alex-spataru/falcon.git master

:: Let's download the changes in the subtrees...
git subtree pull --prefix=lib/LibDS git@github.com:wint-3794/libds.git master
git subtree pull --prefix=lib/Falcon git@github.com:alex-spataru/falcon.git master
