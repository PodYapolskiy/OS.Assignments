Func() {
	date
	sleep 3s
}

Func
mkdir root

Func
touch root/root.txt

Func
mkdir home

Func
touch home/home.txt


cd /
ls -r --sort=time > ~/week01/root/root.txt

cd ~
ls -r --sort=time > ~/week01/home/home.txt

# displaying the content of files
cd week01
echo
echo "Root.txt:"
cat root/root.txt

echo
echo "Home.txt:"
cat home/home.txt

# displaying items of new folders
echo
echo "root/"
ls root/

echo
echo "home/"
ls home/
