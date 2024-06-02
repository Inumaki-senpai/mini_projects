
echo " *** Installing mariadb *** "
apt update && apt install gcc mariadb-common mariadb-client mariadb-server libmariadb-dev -y
echo " ** Installed mariadb ** "


echo " *** Setting up database *** "
mariadb -e "CREATE USER IF NOT EXISTS 'meow'@'localhost' IDENTIFIED BY 'MeowNagar..';"
mariadb -e "GRANT ALL PRIVILEGES ON mini_projects.* TO 'meow'@'localhost';"
mariadb -e "FLUSH PRIVILEGES;"
mariadb < dbsetup.sql
echo " ** Database ready.. ** "


echo " *** Compiling program *** "
gcc montero_quiz.c -o montero_quiz -lmariadbclient
echo " ** Compilation complete ** "


echo " Program installed successfully! "
echo "You can run the program with 'bash montero_quiz' "

exit 0
