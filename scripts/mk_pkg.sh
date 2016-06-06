mkdir pkg
cd pkg
git clone https://srirampc@bitbucket.org/srirampc/arakawa alfred
cd alfred
git submodule init
git submodule update
cd ..
tar cvzf alfred.tar.gz alfred/src/ alfred/ext/ alfred/test/ alfred/data/ alfred/CMakeLists.txt alfred/README
rm -rf alfred
python ../data_pkg.py ../../ ./
cd pkg
tar cvzf datasets.tar.gz data/ runs/ README aliases.xlsx
rm -rf data
rm -rf runs
rm README aliases.xlsx
